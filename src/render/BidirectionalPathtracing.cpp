//
// Created by OpenAI Codex.
//

#include "render/BidirectionalPathtracing.hpp"

#include <algorithm>
#include <cmath>
#include <future>
#include <limits>
#include <thread>
#include <vector>

#include "camera/Camera.hpp"
#include "core/Progressbar.hpp"
#include "core/Random.hpp"
#include "core/Scene.hpp"
#include "core/ThreadPool.hpp"
#include "linearAlgebra/linearAlgebra.hpp"

namespace nagato
{
namespace
{
// サブパス同士を結ぶ際に使うオフセット量。0に近いとセルフシャドウが発生するので注意する。
constexpr float kEpsilon = 1e-3f;
constexpr float kSkyDistance = 1e4f;
}

// ----------------------------------------------------------------------------
// Bidirectional Path Tracing (BDPT)
//   カメラ側と光源側の両方からパスを生成し、可視な頂点同士を結ぶことで
//   Path Tracing では到達しづらい経路の寄与も拾う実装。
// ----------------------------------------------------------------------------

BidirectionalPathtracing::BidirectionalPathtracing(
  Scene *scene,
  Film *film,
  Camera *camera,
  int spp,
  int maxDepth,
  int lightDepth) :
    RenderBase(scene, film, camera), spp(spp), maxDepth(maxDepth), lightDepth(lightDepth)
{
}

void BidirectionalPathtracing::render()
{
  render("output.png");
}

void BidirectionalPathtracing::render(const std::string &outputFilename)
{
  Progressbar prog{spp};

  for (int pass = 0; pass < spp; ++pass)
  {
    render(pass);

    Film temp(film->getWidth(), film->getHeight());
    for (int i = 0; i < film->getWidth() * film->getHeight(); ++i)
    {
      temp[i] = (*film)[i] / static_cast<float>(pass + 1);
    }
    temp.outputImage(outputFilename);

    prog.update();
    prog.printBar();
  }

  for (int i = 0; i < film->getWidth() * film->getHeight(); ++i)
  {
    (*film)[i] = (*film)[i] / static_cast<float>(spp);
  }
  film->outputImage(outputFilename);
}

void BidirectionalPathtracing::render(int current_pass)
{
  const auto width = film->getWidth();
  const auto height = film->getHeight();
  const auto lightPath = generateLightSubpath();

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
  for (int pixel = 0; pixel < width * height; ++pixel)
  {
    const size_t x = pixel % width;
    const size_t y = height - pixel / width;
    const auto L = Li(x, y, lightPath);
    (*film)[pixel] = (*film)[pixel] + L;
  }
#else
  const auto thread_num = std::max(1u, std::thread::hardware_concurrency());
  ThreadPool pool(thread_num, thread_num);
  std::vector<std::future<void>> futures;
  futures.reserve(height);

  for (int yIdx = 0; yIdx < height; ++yIdx)
  {
    futures.emplace_back(pool.enqueue_task(
      [this, yIdx, width, height, &lightPath]()
      {
        const size_t y = height - yIdx;
        for (size_t x = 0; x < width; ++x)
        {
          const auto L = Li(x, y, lightPath);
          const size_t index = yIdx * width + x;
          (*film)[index] = (*film)[index] + L;
        }
      }));
  }

  for (auto &future : futures)
  {
    future.get();
  }
#endif
}

Spectrum BidirectionalPathtracing::Li(size_t x, size_t y, const std::vector<PathVertex> &lightPath)
{
  // カメラ側で直接計算できる寄与成分（NEEや環境光）
  Spectrum directContribution(0.0f);
  Spectrum emissionContribution(0.0f);

  auto cameraPath = generateCameraSubpath(x, y, directContribution, emissionContribution);

  Spectrum connectionContribution(0.0f);
  if (!cameraPath.empty() && !lightPath.empty())
  {
    connectionContribution = connectPaths(cameraPath, lightPath);
  }

  return directContribution + emissionContribution + connectionContribution;
}

std::vector<BidirectionalPathtracing::PathVertex> BidirectionalPathtracing::generateCameraSubpath(
  size_t x,
  size_t y,
  Spectrum &directLighting,
  Spectrum &emissionContribution)
{
  // カメラ側からのパスを逐次発展させる（NEE や環境光はここで加算）。
  std::vector<PathVertex> path;
  path.reserve(maxDepth);

  // 各頂点までのスループット（重要度）を持ち回る
  Spectrum throughput(1.0f);
  auto ray = camera->makePrimaryRay(x, y);

  for (int depth = 0; depth < maxDepth; ++depth)
  {
    const auto intersect = scene->intersect(ray, 0.0f, std::numeric_limits<float>::max());
    if (!intersect)
    {
      if (scene->hasSky())
      {
        emissionContribution = emissionContribution + throughput * scene->getSky().getRadiance(ray);
      }
      break;
    }

    // ヒット情報をサブパス頂点として整形
    PathVertex vertex;
    vertex.hit = intersect.value();
    vertex.throughput = throughput;
    vertex.material = &intersect->getObject().getMaterial();
    vertex.wo = -ray.getDirection();
    vertex.isEmitter = vertex.material->type() == SurfaceType::Emitter;
    vertex.isDelta = isDeltaSurface(vertex.material);
    path.push_back(vertex);

    if (vertex.isEmitter)
    {
      emissionContribution = emissionContribution + throughput * vertex.material->getEmitter();
    }

    if (!vertex.isEmitter && vertex.material->type() == SurfaceType::Diffuse)
    {
      directLighting = directLighting + throughput * scene->directLight(ray, intersect.value());
    }

    ray.setOrigin(vertex.hit.getPoint());

    Vector3 newDir;
    float pdf = 1.0f;
    int wavelength = -1;
    auto &bsdf = vertex.material->getBSDF();
    auto bsdfValue = bsdf.makeNewDirection(&wavelength, &newDir, ray, intersect.value(), &pdf);
    bsdfValue = bsdfValue * vertex.material->getTextureColor(&intersect.value());
    const float cosTerm = std::abs(dot(newDir, intersect->getNormal()));

    if (pdf <= 0.0f || cosTerm <= 0.0f)
    {
      break;
    }

    throughput = throughput * (bsdfValue * (cosTerm / pdf));

    const float maxComponent = throughput.findMaxSpectrum();
    if (maxComponent == 0.0f)
    {
      break;
    }

    if (depth >= 3)
    {
      // パスが長くなりすぎないようロシアンルーレットで打ち切り確率を調整
      const float rr = std::min(maxComponent, 0.9f);
      if (Random::Instance().next() > rr)
      {
        break;
      }
      throughput = throughput / rr;
    }

    ray.setDirection(normalize(newDir));
    ray.setOrigin(ray.getOrigin() + ray.getDirection() * kEpsilon);
  }

  return path;
}

std::vector<BidirectionalPathtracing::PathVertex> BidirectionalPathtracing::generateLightSubpath()
{
  const auto &lights = scene->getLights();
  const bool hasSky = scene->hasSky();
  const bool hasArea = !lights.empty();

  if (!hasArea && !hasSky)
  {
    return {};
  }

  float skySelectProb = hasSky ? (hasArea ? 0.5f : 1.0f) : 0.0f;
  float areaSelectProb = hasArea ? (1.0f - skySelectProb) : 0.0f;

  if (Random::Instance().next() < skySelectProb)
  {
    return generateSkySubpath(skySelectProb);
  }

  if (!hasArea)
  {
    return {};
  }

  return generateAreaLightSubpath(areaSelectProb);
}

std::vector<BidirectionalPathtracing::PathVertex> BidirectionalPathtracing::generateAreaLightSubpath(
  float selectProbability)
{
  std::vector<PathVertex> path;
  if (selectProbability <= 0.0f)
  {
    return path;
  }

  path.reserve(lightDepth);
  const auto &lights = scene->getLights();

  float totalArea = 0.0f;
  for (const auto *light : lights)
  {
    totalArea += light->area();
  }

  if (totalArea <= 0.0f)
  {
    return path;
  }

  const float selector = Random::Instance().next() * totalArea;
  Object *selectedLight = lights.front();
  float accumulator = 0.0f;
  for (auto *light : lights)
  {
    accumulator += light->area();
    if (selector <= accumulator)
    {
      selectedLight = light;
      break;
    }
  }

  const float selectPdf = (selectedLight->area() / totalArea) * selectProbability;
  const float areaPdf = 1.0f / selectedLight->area();

  auto sampled = selectedLight->sampleSurfacePoint();
  auto normal = normalize(sampled.getNormal());
  auto dir = sampleCosineHemisphere(normal);
  const float cosTheta = std::max(0.0f, dot(normal, dir));
  if (cosTheta <= 0.0f)
  {
    return path;
  }

  const float dirPdf = cosTheta / static_cast<float>(M_PI);
  const float pdf = selectPdf * areaPdf * dirPdf;
  if (pdf <= 0.0f)
  {
    return path;
  }

  Spectrum throughput = selectedLight->getMaterial().getEmitter() * (cosTheta / pdf);
  Ray ray(sampled.getPoint() + normal * kEpsilon, dir);

  for (int depth = 0; depth < lightDepth; ++depth)
  {
    const auto intersect = scene->intersect(ray, 0.0f, std::numeric_limits<float>::max());
    if (!intersect)
    {
      break;
    }

    PathVertex vertex;
    vertex.hit = intersect.value();
    vertex.material = &intersect->getObject().getMaterial();
    vertex.throughput = throughput;
    vertex.wo = -ray.getDirection();
    vertex.isEmitter = vertex.material->type() == SurfaceType::Emitter;
    vertex.isDelta = isDeltaSurface(vertex.material);
    path.push_back(vertex);

    ray.setOrigin(vertex.hit.getPoint());

    Vector3 newDir;
    float pdfBsdf = 1.0f;
    int wavelength = -1;
    auto &bsdf = vertex.material->getBSDF();
    auto bsdfValue = bsdf.makeNewDirection(&wavelength, &newDir, ray, intersect.value(), &pdfBsdf);
    bsdfValue = bsdfValue * vertex.material->getTextureColor(&intersect.value());
    const float cosTerm = std::abs(dot(newDir, intersect->getNormal()));
    if (pdfBsdf <= 0.0f || cosTerm <= 0.0f)
    {
      break;
    }

    throughput = throughput * (bsdfValue * (cosTerm / pdfBsdf));

    const float maxComponent = throughput.findMaxSpectrum();
    if (maxComponent == 0.0f)
    {
      break;
    }

    if (depth >= 3)
    {
      const float rr = std::min(maxComponent, 0.9f);
      if (Random::Instance().next() > rr)
      {
        break;
      }
      throughput = throughput / rr;
    }

    ray.setDirection(normalize(newDir));
    ray.setOrigin(ray.getOrigin() + ray.getDirection() * kEpsilon);
  }

  return path;
}

std::vector<BidirectionalPathtracing::PathVertex> BidirectionalPathtracing::generateSkySubpath(
  float selectProbability)
{
  std::vector<PathVertex> path;
  if (!scene->hasSky() || selectProbability <= 0.0f)
  {
    return path;
  }

  path.reserve(lightDepth);
  auto dir = sampleUniformSphere();
  Vector3 origin = -dir * kSkyDistance;
  Ray ray(origin, dir);

  const float dirPdf = 1.0f / (4.0f * static_cast<float>(M_PI));
  const float pdf = selectProbability * dirPdf;
  if (pdf <= 0.0f)
  {
    return path;
  }

  Spectrum throughput = scene->getSky().getRadiance(Ray(Vector3(0.0f), dir)) / pdf;

  for (int depth = 0; depth < lightDepth; ++depth)
  {
    const auto intersect = scene->intersect(ray, 0.0f, std::numeric_limits<float>::max());
    if (!intersect)
    {
      break;
    }

    PathVertex vertex;
    vertex.hit = intersect.value();
    vertex.material = &intersect->getObject().getMaterial();
    vertex.throughput = throughput;
    vertex.wo = -ray.getDirection();
    vertex.isEmitter = vertex.material->type() == SurfaceType::Emitter;
    vertex.isDelta = isDeltaSurface(vertex.material);
    path.push_back(vertex);

    ray.setOrigin(vertex.hit.getPoint());

    Vector3 newDir;
    float pdfBsdf = 1.0f;
    int wavelength = -1;
    auto &bsdf = vertex.material->getBSDF();
    auto bsdfValue = bsdf.makeNewDirection(&wavelength, &newDir, ray, intersect.value(), &pdfBsdf);
    bsdfValue = bsdfValue * vertex.material->getTextureColor(&intersect.value());
    const float cosTerm = std::abs(dot(newDir, intersect->getNormal()));
    if (pdfBsdf <= 0.0f || cosTerm <= 0.0f)
    {
      break;
    }

    throughput = throughput * (bsdfValue * (cosTerm / pdfBsdf));

    const float maxComponent = throughput.findMaxSpectrum();
    if (maxComponent == 0.0f)
    {
      break;
    }

    if (depth >= 3)
    {
      const float rr = std::min(maxComponent, 0.9f);
      if (Random::Instance().next() > rr)
      {
        break;
      }
      throughput = throughput / rr;
    }

    ray.setDirection(normalize(newDir));
    ray.setOrigin(ray.getOrigin() + ray.getDirection() * kEpsilon);
  }

  return path;
}

Spectrum BidirectionalPathtracing::connectPaths(
  const std::vector<PathVertex> &cameraPath,
  const std::vector<PathVertex> &lightPath)
{
  Spectrum accumulated(0.0f);

  // δ分布以外の頂点同士を全探索で接続し、可視であれば寄与を加算する。
  for (const auto &cameraVertex : cameraPath)
  {
    if (cameraVertex.isDelta)
      continue;

    for (const auto &lightVertex : lightPath)
    {
      if (lightVertex.isDelta)
        continue;

      accumulated = accumulated + connectVertices(cameraVertex, lightVertex);
    }
  }

  return accumulated;
}

Spectrum BidirectionalPathtracing::connectVertices(
  const PathVertex &cameraVertex,
  const PathVertex &lightVertex) const
{
  // 2 頂点を結ぶセグメントを可視判定し、幾何項と BSDF を用いて寄与を計算する。
  const auto start = cameraVertex.hit.getPoint();
  const auto end = lightVertex.hit.getPoint();
  Vector3 toLight = end - start;
  const float distanceSquared = toLight.norm();
  if (distanceSquared <= 0.0f)
  {
    return Spectrum(0.0f);
  }

  const float distance = std::sqrt(distanceSquared);
  Vector3 dir = normalize(toLight);

  const float cosCamera = std::max(0.0f, dot(cameraVertex.hit.getNormal(), dir));
  const float cosLight = std::max(0.0f, dot(lightVertex.hit.getNormal(), -dir));
  if (cosCamera <= 0.0f || cosLight <= 0.0f)
  {
    return Spectrum(0.0f);
  }

  Ray shadowRay(start + dir * kEpsilon, dir);
  const auto occlusion = scene->intersect(shadowRay, 0.0f, distance - kEpsilon);
  if (occlusion)
  {
    // 区間内に遮蔽物がある場合は寄与なし
    return Spectrum(0.0f);
  }

  // カメラ側の BSDF と光側の BSDF（または放射輝度）を取得
  Spectrum cameraBsdf = cameraVertex.material->getBSDF().f_r(cameraVertex.wo, dir);
  cameraBsdf = cameraBsdf * cameraVertex.material->getTextureColor(&cameraVertex.hit);
  Spectrum lightBsdf(1.0f);

  if (!lightVertex.isEmitter)
  {
    lightBsdf = lightVertex.material->getBSDF().f_r(lightVertex.wo, -dir);
    lightBsdf = lightBsdf * lightVertex.material->getTextureColor(&lightVertex.hit);
  }
  else
  {
    lightBsdf = lightVertex.material->getEmitter();
  }

  // 幾何項 G(x,y) = (cosθ_x * cosθ_y) / ||x-y||^2
  const float geometry = (cosCamera * cosLight) / distanceSquared;
  return cameraVertex.throughput * cameraBsdf * lightVertex.throughput * lightBsdf * geometry;
}

bool BidirectionalPathtracing::isDeltaSurface(const Material *material)
{
  if (material == nullptr)
  {
    return false;
  }

  const auto type = material->type();
  return type == SurfaceType::Mirror || type == SurfaceType::Fresnel;
}

void BidirectionalPathtracing::buildOrthonormalBasis(const Vector3 &n, Vector3 &t, Vector3 &b)
{
  // 与えられた法線 n から接ベクトルと従法線を構築し、局所座標系を生成する。
  if (std::abs(n.x) > std::abs(n.z))
  {
    t = normalize(Vector3(-n.y, n.x, 0.0f));
  }
  else
  {
    t = normalize(Vector3(0.0f, -n.z, n.y));
  }
  b = cross(n, t);
}

Vector3 BidirectionalPathtracing::sampleCosineHemisphere(const Vector3 &normal)
{
  // 法線を基準とした余弦加重サンプリング。デルタ面以外の拡散面で利用する。
  auto &rng = Random::Instance();
  const float r1 = rng.next();
  const float r2 = rng.next();

  const float phi = 2.0f * static_cast<float>(M_PI) * r1;
  const float r = std::sqrt(r2);
  const float x = r * std::cos(phi);
  const float y = r * std::sin(phi);
  const float z = std::sqrt(std::max(0.0f, 1.0f - r2));

  Vector3 tangent, bitangent;
  buildOrthonormalBasis(normal, tangent, bitangent);

  return normalize(tangent * x + bitangent * y + normal * z);
}

Vector3 BidirectionalPathtracing::sampleUniformSphere()
{
  auto &rng = Random::Instance();
  const float u = rng.next();
  const float v = rng.next();
  const float z = 1.0f - 2.0f * u;
  const float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
  const float phi = 2.0f * static_cast<float>(M_PI) * v;
  const float x = r * std::cos(phi);
  const float y = r * std::sin(phi);
  return Vector3(x, y, z);
}
} // namespace nagato
