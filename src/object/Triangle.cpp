//
// Created by 長井亨 on 2018/07/28.
//

#include "object/Triangle.hpp"
#include <cmath>
#include <utility>

namespace nagato
{
Triangle::Triangle(Material *m, std::vector<Vector3> p) : Object(m), points(std::move(p)) {}

std::optional<Hit> Triangle::intersect(Ray &ray, float tmin, float tmax)
{
  const auto origin = ray.getOrigin();
  constexpr auto epsilon = 0.000001;

  const auto edge1 = points[1] - points[0];
  const auto edge2 = points[2] - points[0];

  const auto P = cross(ray.getDirection(), edge2);
  const float det = dot(P, edge1);

  if (det >= 0.0f)
  {
    Vector3 T = origin - points[0];
    float u = dot(P, T);

    if (u >= 0.0 && u <= 1.0 * det)
    {
      const auto Q = cross(T, edge1);
      const auto v = dot(Q, ray.getDirection());

      if (v >= 0.0 && (u + v) <= 1.0 * det)
      {
        auto t = dot(Q, edge2) / det;
        auto hitpoint = origin + ray.getDirection() * t;
        const auto normal = normalize(cross(points[1] - points[0], points[2] - points[1]));
        if (0.0f <= t && t <= tmax)
        {
          return Hit{t, hitpoint, normal, this};
        }
      }
    }
  }

  return std::nullopt;
}

Aabb Triangle::getAABB() const
{
  Vector3 min(MAXFLOAT);
  Vector3 max(-MAXFLOAT);

  for (int i = 0; i < 3; i++)
  {
    if (min.x > points[i].x)
    {
      min.x = points[i].x;
    }
    if (min.y > points[i].y)
    {
      min.y = points[i].y;
    }
    if (min.z > points[i].z)
    {
      min.z = points[i].z;
    }

    if (max.x < points[i].x)
    {
      max.x = points[i].x;
    }
    if (max.y < points[i].y)
    {
      max.y = points[i].y;
    }
    if (max.z < points[i].z)
    {
      max.z = points[i].z;
    }
  }

  return Aabb(min, max);
}

Hit Triangle::pointSampling(Hit surfaceInfo)
{
  float xi1 = Random::Instance().next();
  float xi2 = Random::Instance().next();

  float u = 1 - std::sqrt(xi1);
  float v = xi2 * std::sqrt(xi1);

  // サンプリングされた座標
  auto sampledPoint = points[0] * u + points[1] * v + points[2] * (1 - u - v);

  // サンプリングされた座標と
  auto distance = std::sqrt((sampledPoint - surfaceInfo.getPoint()).norm());

  // サンプリング地点の法線ベクトル
  auto normal = normalize(cross(points[1] - points[0], points[2] - points[1]));

  return Hit{distance, sampledPoint, normal, this};
}

std::string Triangle::toString() const { return "[Triangle]material : " + material->typeName(); }

float Triangle::area() const
{
  auto A = points[2] - points[0];
  auto B = points[1] - points[0];

  auto AB_cross = cross(A, B);

  return std::sqrt(AB_cross.norm()) / 2.0f;
}
} // namespace nagato
