//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SPHERE_HPP
#define PATHTRACING_SPHERE_HPP

#include <optional>
#include "Object.hpp"
#include "Hit.hpp"
#include "Ray.hpp"

namespace nagato {
    class Sphere : public Object {
    public:
        double radius;

        Sphere(Vector3 p, double r, SurfaceType t, Vector3 color, Vector3 em = Vector3())
                : Object(p, t, color, em), radius(r) {}

        std::optional<Hit> intersect(
                Ray &ray,
                double tmin,
                double tmax) override {
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
    };
}
#endif //PATHTRACING_SPHERE_HPP
