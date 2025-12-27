//
// Created by 長井亨 on 2018/07/28.
//

#include "structure/Aabb.hpp"
#include <float.h>
namespace nagato
{
Aabb::Aabb(Vector3 min, Vector3 max) : min(min), max(max) {}

bool Aabb::intersect(Ray &ray)
{
  float t_max = FLT_MAX; // AABB からレイが外に出る時刻
  float t_min = -FLT_MAX; // AABB にレイが侵入する時刻

  const Vector3 &origin = ray.getOrigin();
  const Vector3 &direction = ray.getDirection();

  for (int i = 0; i < 3; i++)
  {
    if (std::abs(direction[i]) < 1e-8f)
    {
      if (origin[i] < min[i] || origin[i] > max[i])
        return false;
    }
    else
    {
      float invDir = 1.0f / direction[i];
      float t1 = (min[i] - origin[i]) * invDir;
      float t2 = (max[i] - origin[i]) * invDir;
      float t_near = std::min(t1, t2);
      float t_far = std::max(t1, t2);
      t_max = std::min(t_max, t_far);
      t_min = std::max(t_min, t_near);
    }

    // レイが外に出る時刻と侵入する時刻が逆転している => 交差していない
    if (t_min > t_max)
      return false;
  }
  return true;
}

Aabb::Aabb() :
    min(Vector3(std::numeric_limits<float>::max())),
    max(Vector3(std::numeric_limits<float>::lowest()))
{
}

Vector3 Aabb::getCenter()
{
  return Vector3{static_cast<float>((min.x + max.x) / 2.0),
                 static_cast<float>((min.y + max.y) / 2.0),
                 static_cast<float>((min.z + max.z) / 2.0)};
}
} // namespace nagato
