//
// Created by 長井亨 on 2018/07/18.
//
#include <cmath>
#include "Sphere.hpp"

namespace nagato {

    std::optional <Hit> Sphere::intersect(Ray &ray, double tmin, double tmax) {
      const Vector3 op = position - ray.origin;
      const double b = dot(op, ray.direction);
      const double det = b * b - dot(op, op) + radius * radius;

      if (det < 0) {
        return {};
      }
      const double t1 = b - sqrt(det);

      if (tmin < t1 && t1 < tmax) {
        auto point = ray.origin + ray.direction * t1;
        auto normal = (point - position) / radius;
        return Hit{t1, point, normal, this};
      }

      const double t2 = b + sqrt(det);
      if (tmin < t2 && t2 < tmax) {
        auto point = ray.origin + ray.direction * t2;
        auto normal = (point - position) / radius;
        return Hit{t2, point, normal, this};
      }
      return {};
    }

    Sphere::Sphere(Vector3 p, double r, SurfaceType t, Vector3 color, Vector3 em)
            : Object(p, t, color, em), radius(r) {}
}