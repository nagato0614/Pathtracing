//
// Created by 長井亨 on 2018/09/06.
//

#include "BSDF/Lambert.hpp"

namespace nagato
{
Lambert::Lambert(Spectrum c) : BSDF(c) {}

Spectrum Lambert::makeNewDirection(
  int *wavelengthIndex, Vector3 *newDirection, Ray &ray, const Hit &surfaceInfo, float *pdf) const
{
  // Sample direction in local coordinates
  const auto n = dot(surfaceInfo.getNormal(), -ray.getDirection()) > 0 ? surfaceInfo.getNormal()
                                                                       : -surfaceInfo.getNormal();
  const auto &[u, v] = tangentSpace(n);
  const auto d = [&]()
  {
    const auto r = sqrt(Random::Instance().next());
    const auto t = 2 * M_PI * Random::Instance().next();
    const auto x = r * cos(t);
    const auto y = r * sin(t);
    return Vector3(
      (float) x,
      (float) y,
      std::sqrt(std::max(float(0.0), static_cast<const float &>(1.0 - x * x - y * y))));
  }();
  // Convert to world coordinates
  *newDirection = u * d.x + v * d.y + n * d.z;
  *pdf = this->pdf(-ray.getDirection(), *newDirection, surfaceInfo);
  return f_r(-ray.getDirection(), *newDirection);
}

Spectrum Lambert::f_r(const Vector3 &wi, const Vector3 &wo) const
{
  return color * this->f(wi, wo);
}

float Lambert::pdf(const Vector3 &wi, const Vector3 &wo, const Hit &hitPoint) const
{
  auto cos = dot(wo, hitPoint.getNormal());
  return cos / M_PI;
}

float Lambert::f(const Vector3 &wi, const Vector3 &wo) const { return 1.0f / M_PI; }

float Lambert::pdf(const Vector3 &wi, const Vector3 &wo) const
{
  auto cos = dot(wo, Vector3{0, 0, 1.0});
  return cos * M_1_PI;
}

std::shared_ptr<Lambert> createLambert(Spectrum c) { return std::make_shared<Lambert>(c); }
} // namespace nagato
