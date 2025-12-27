//
// Created by 長井亨 on 2018/09/05.
//

#ifndef PATHTRACING_DIFFUSELIGHT_HPP
#define PATHTRACING_DIFFUSELIGHT_HPP

#include "Light.hpp"

namespace nagato
{
class DiffuseLight : Light
{
  public:
    DiffuseLight(Spectrum I, LightType type, Object *o);
    Spectrum samplePoint(Hit hit, Vector3 *sampledPoint, float *pdf) override;

  private:
    Object *object;
};
} // namespace nagato

#endif // PATHTRACING_DIFFUSELIGHT_HPP
