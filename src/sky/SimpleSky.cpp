//
// Created by 長井亨 on 2019-02-06.
//

#include "sky/SimpleSky.hpp"
#include <cmath>
#include "core/Random.hpp"

namespace nagato
{

Spectrum SimpleSky::getRadiance(const Ray &ray) const
{
  float t = (ray.getDirection().y + 1) / 2.0f;
  return (1 - t) * Spectrum(1) + t * skyColor;
}

SimpleSky::SimpleSky(const Spectrum &s) : skyColor(s) {}

SkySample SimpleSky::sample(const Vector3 &origin) const
{
  const Vector3 direction = sampleUniformSphere();
  SkySample sample;
  sample.ray = Ray(origin, direction);
  sample.radiance = getRadiance(Ray(origin, direction));
  sample.pdf = 1.0f / (4.0f * static_cast<float>(M_PI));
  return sample;
}
} // namespace nagato
