//
// Created by 長井亨 on 2019-02-04.
//

#include "sky/UniformSky.hpp"
#include <cmath>
#include "core/Random.hpp"

namespace nagato
{

Spectrum UniformSky::getRadiance(const Ray &ray) const { return skyColor; }

UniformSky::UniformSky(const Spectrum &s) : skyColor(s) {}

SkySample UniformSky::sample(const Vector3 &origin) const
{
  SkySample sample;
  const Vector3 direction = sampleUniformSphere();
  sample.ray = Ray(origin, direction);
  sample.radiance = skyColor;
  sample.pdf = 1.0f / (4.0f * static_cast<float>(M_PI));
  return sample;
}
} // namespace nagato
