//
// Created by 長井亨 on 2018/07/18.
//

#include "core/Hit.hpp"

namespace nagato
{
Hit::Hit(float d, Vector3 p, Vector3 n, Object *sphere, std::optional<Vector2> tex) :
    distance(d), point(p), normal(n), object(sphere), texcoord(std::move(tex))
{
}

Hit::Hit() {}

const float Hit::getDistance() const { return distance; }

const Vector3 &Hit::getPoint() const { return point; }

const Vector3 &Hit::getNormal() const { return normal; }

const Object &Hit::getObject() const { return *object; }

bool Hit::hasTexcoord() const { return texcoord.has_value(); }

const Vector2 &Hit::getTexcoord() const
{
  static Vector2 dummy(0.0f);
  if (!texcoord)
  {
    return dummy;
  }
  return texcoord.value();
}
} // namespace nagato
