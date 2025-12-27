//
// Created by 長井亨 on 2018/09/04.
//

#include "light/PointLight.hpp"

namespace nagato
{
PointLight::PointLight(Spectrum I, LightType type, Vector3 point) : Light(I, type), point(point) {}

Spectrum PointLight::samplePoint(Hit hit, Vector3 *sampledPoint, float *pdf)
{
  *sampledPoint = point - hit.getPoint();
  *pdf = 1.f;
  return I;
}
} // namespace nagato
