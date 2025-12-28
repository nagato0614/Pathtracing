//
// Created by 長井亨 on 2019-02-04.
//

#ifndef PATHTRACING_SKY_HPP
#define PATHTRACING_SKY_HPP

#include "color/Spectrum.hpp"
#include "core/Ray.hpp"

namespace nagato
{
struct SkySample
{
    Ray ray;
    Spectrum radiance;
    float pdf = 0.0f;
};

class Sky
{
  public:
    virtual ~Sky() = default;
    virtual Spectrum getRadiance(const Ray &ray) const = 0;
    virtual SkySample sample(const Vector3 &origin) const = 0;

  protected:
    static Vector3 sampleUniformSphere();
};
} // namespace nagato

#endif // PATHTRACING_SKY_HPP
