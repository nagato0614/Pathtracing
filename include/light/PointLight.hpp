//
// Created by 長井亨 on 2018/09/04.
//

#ifndef PATHTRACING_POINTLIGHT_HPP
#define PATHTRACING_POINTLIGHT_HPP

#include "light/Light.hpp"

namespace nagato
{
class PointLight : Light
{
  public:
    PointLight(Spectrum I, LightType type, Vector3 point);

    Spectrum samplePoint(Hit hit, Vector3 *sampledPoint, float *pdf) override;

  private:
    Vector3 point;
};
} // namespace nagato

#endif // PATHTRACING_POINTLIGHT_HPP
