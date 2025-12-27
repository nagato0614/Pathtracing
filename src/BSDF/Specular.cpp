//
// Created by 長井亨 on 2018/09/06.
//

#include "BSDF/Specular.hpp"

namespace nagato
{

Specular::Specular(Spectrum c) : BSDF(c) {}

Spectrum Specular::makeNewDirection(
  int *wavelengthIndex, Vector3 *newDirection, Ray &ray, const Hit &surfaceInfo, float *pdf) const
{
  const auto wi = -ray.getDirection();
  *newDirection = surfaceInfo.getNormal() * 2 * dot(wi, surfaceInfo.getNormal()) - wi;
  *pdf = 1.0;
  return this->color;
}

std::shared_ptr<Specular> createSpecular(Spectrum c) { return std::make_shared<Specular>(c); }

} // namespace nagato
