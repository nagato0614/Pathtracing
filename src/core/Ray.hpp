//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_RAY_HPP
#define PATHTRACING_RAY_HPP

#include "../linearAlgebra/Vector3.hpp"

namespace nagato
{
class Ray
{
  public:
    Ray() = default;

    Ray(Vector3 o, Vector3 dir) : origin(o), direction(dir) {}

    const auto &getOrigin() const { return origin; }

    const auto &getDirection() const { return direction; }

    void setOrigin(const Vector3 &o) { this->origin = o; }

    void setDirection(const Vector3 &dir) { this->direction = dir; }

  private:
    Vector3 origin;
    Vector3 direction;
};
} // namespace nagato

#endif // PATHTRACING_RAY_HPP
