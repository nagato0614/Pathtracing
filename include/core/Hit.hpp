//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_HIT_HPP
#define PATHTRACING_HIT_HPP

#include <optional>
#include "linearAlgebra/Vector2.hpp"
#include "object/Object.hpp"

namespace nagato
{
class Object;

class Hit
{
  public:
    Hit();

    Hit(float d, Vector3 p, Vector3 n, Object *sphere, std::optional<Vector2> tex = std::nullopt);

    const float getDistance() const;

    const Vector3 &getPoint() const;

    const Vector3 &getNormal() const;

    const Object &getObject() const;

    bool hasTexcoord() const;

    const Vector2 &getTexcoord() const;

  private:
    float distance;
    Vector3 point;
    Vector3 normal;
    Object *object;
    std::optional<Vector2> texcoord;
};
} // namespace nagato

#endif // PATHTRACING_HIT_HPP
