//
// Created by 長井亨 on 2019-01-30.
//

#include "render/Pathtracing.hpp"
#include <future>
#include "core/Progressbar.hpp"
#include "core/ThreadPool.hpp"

namespace nagato
{

Pathtracing::Pathtracing(Scene *scene, Film *film, Camera *camera, int spp, int depth) :
    RenderBase(scene, film, camera), spp(spp), maxDepth(depth)
{
}

void Pathtracing::render()
{
  Progressbar prog{spp};

  for (int pass = 0; pass < spp; pass++)
  {
    render(pass);

    // 平均化して出力
    Film temp_film(film->getWidth(), film->getHeight());
    for (int i = 0; i < film->getWidth() * film->getHeight(); i++)
    {
      temp_film[i] = (*film)[i] / (float) (pass + 1);
    }
    temp_film.outputImage("output.png");

    prog.update();
    prog.printBar();
  }

  // Final average for last output
  for (int i = 0; i < film->getWidth() * film->getHeight(); i++)
  {
    (*film)[i] = (*film)[i] / (float) spp;
  }
  film->outputImage("output.png");
}

void Pathtracing::render(int current_pass)
{
  const auto width = film->getWidth();
  const auto height = film->getHeight();

#ifndef _OPENMP
  const auto thread_num = std::thread::hardware_concurrency() - 4;
  ThreadPool pool(thread_num, thread_num);
#endif

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
  for (int i = 0; i < width * height; i++)
  {
    const size_t x = i % width;
    const size_t y = height - i / width;

    const auto L = Li(x, y);

    (*film)[i] = (*film)[i] + L;
  }
#else
  std::vector<std::future<void>> futures;
  for (int y_idx = 0; y_idx < height; y_idx++)
  {
    futures.emplace_back(pool.enqueue_task(
      [this, y_idx, width, height]()
      {
        const size_t y = height - y_idx;
        for (size_t x = 0; x < width; x++)
        {
          const auto L = Li(x, y);
          const size_t i = y_idx * width + x;
          (*film)[i] = (*film)[i] + L;
        }
      }));
  }
  for (auto &f : futures)
  {
    f.get();
  }
#endif
}

Spectrum Pathtracing::Li(size_t x, size_t y)
{
  auto ray = camera->makePrimaryRay(x, y);

  // スペクトルの最終的な寄与
  Spectrum L(0.0);

  // 各パスにおけるウェイト
  Spectrum weight(1.0);

  // 波長ごとの追跡を行う場合における追跡している波長
  int wavelength = -1;

  for (int depth = 0; depth < maxDepth; depth++)
  {
    const auto &intersect = scene->intersect(ray, 0.0f, 1e+100);

    if (!intersect)
    {
      if (scene->hasSky())
        L = L + weight * scene->getSky().getRadiance(ray);
      break;
    }

    // ヒットした面が表の場合光源の寄与を得る
    if (dot(-ray.getDirection(), intersect->getNormal()) > 0.0)
    {
      L = L + weight * intersect->getObject().getMaterial().getEmitter();
    }

    // next event estimation
    auto type = intersect->getObject().getMaterial().type();
    if (type != SurfaceType::Emitter && type == SurfaceType::Diffuse)
    {
      L = L + weight * scene->directLight(ray, intersect.value());
    }

    // 次の経路を構築
    ray.setOrigin(intersect->getPoint());
    Vector3 dir;
    float pdf = 1.0;
    auto &bsdf = intersect->getObject().getMaterial().getBSDF();
    auto color = bsdf.makeNewDirection(&wavelength, &dir, ray, intersect.value(), &pdf);

    // スループットウェイトの更新
    weight = weight * ((color * std::abs(dot(-ray.getDirection(), dir))) / pdf);
    if (weight.findMaxSpectrum() == 0)
    {
      break;
    }

    ray.setDirection(dir);

    // ロシアンルーレットで追跡を終了する
    auto maxWeight = weight.findMaxSpectrum();
    if (Random::Instance().next() < 1.0 - maxWeight && depth > 3)
    {
      break;
    }
    else
    {
      weight = weight / maxWeight;
    }
  }

  return L;
}
} // namespace nagato
