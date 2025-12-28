//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SPHERE_HPP
#define PATHTRACING_SPHERE_HPP

#include <optional>
#include "object/Object.hpp"

namespace nagato
{
class Vector3;

class Sphere : public Object
{
  public:
    float radius;
    Vector3 position;

    Sphere(Vector3 p, float r, Material *m);

    std::optional<Hit> intersect(Ray &ray, float tmin, float tmax) override;

    Aabb getAABB() const override;

    Hit pointSampling(Hit surfaceInfo) override;

    Hit sampleSurfacePoint() const override;

    std::string toString() const override;

    float area() const override;
};
} // namespace nagato
#endif // PATHTRACING_SPHERE_HPP
