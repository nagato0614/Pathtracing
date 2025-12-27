//
// Created by 長井亨 on 2018/09/03.
//

#ifndef PATHTRACING_LIGHT_HPP
#define PATHTRACING_LIGHT_HPP

#include "../color/Spectrum.hpp"
#include "../core/Hit.hpp"

namespace nagato
{
enum LightType
{
  None,
  Point,
  SpotLight,
  DiffuseLight
};

class Light
{
  public:
    Light(Spectrum I, LightType type) : I(I), lightType(type) {}

    LightType type() { return lightType; }

    virtual Spectrum samplePoint(Hit hit, Vector3 *sampledPoint, float *pdf) = 0;

  protected:
    Spectrum I;
    LightType lightType;
};
} // namespace nagato

#endif // PATHTRACING_LIGHT_HPP
