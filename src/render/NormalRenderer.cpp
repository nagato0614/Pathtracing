//
// 法線のみを出力するデバッグレンダラー
//

#include "render/NormalRenderer.hpp"

#include <iostream>
#include <limits>

#include "camera/Camera.hpp"
#include "film/Film.hpp"
#include "structure/BVH.hpp"

namespace nagato
{

NormalRenderer::NormalRenderer(Scene *scene, Film *film, Camera *camera) : RenderBase(scene, film, camera) {}

void NormalRenderer::render() { render(0); }

void NormalRenderer::render(int current_pass)
{
  (void) current_pass;

  const auto width = film->getWidth();
  const auto height = film->getHeight();
  bool firstRayReported = false;

  for (size_t y = 0; y < height; ++y)
  {
    for (size_t x = 0; x < width; ++x)
    {
      auto ray = camera->makePrimaryRay(x, height - y);
      const auto hit = scene->intersect(ray, 0.0f, std::numeric_limits<float>::max());
      Spectrum value(0.0f);

      if (!firstRayReported)
      {
        firstRayReported = true;
        if (hit)
        {
          const Vector3 normal = normalize(hit->getNormal());
          std::cout << "[NormalRenderer] First primary ray normal: " << normal.toString()
                    << std::endl;
        }
        else
        {
          std::cout << "[NormalRenderer] First primary ray missed; no normal available."
                    << std::endl;
        }
      }
      if (hit)
      {
        const Vector3 normal = normalize(hit->getNormal());
        const Vector3 mapped = (normal + Vector3(1.0f)) * 0.5f;
        value[0] = mapped.x;
        value[1] = mapped.y;
        value[2] = mapped.z;
      }
      film->addSample(value, y * width + x);
    }
  }
}

void NormalRenderer::render(const std::string &outputFilename)
{
  render();
  film->outputImage(outputFilename);
}

} // namespace nagato
