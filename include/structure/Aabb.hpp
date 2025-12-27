//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_AABB_HPP
#define PATHTRACING_AABB_HPP

#include "core/Ray.hpp"
#include "linearAlgebra/Vector3.hpp"

namespace nagato
{
class Aabb
{
  public:
    Aabb();

    Aabb(Vector3 min, Vector3 max);

    bool intersect(Ray &ray);

    Vector3 getCenter();

    Vector3 min;
    Vector3 max;
};
} // namespace nagato

#endif // PATHTRACING_AABB_HPP
