//
// Created by 長井亨 on 2018/07/18.
//
#include "Sphere.hpp"
#include <cmath>

namespace nagato
{

    std::optional<Hit> Sphere::intersect(Ray &ray, float tmin, float tmax)
    {
        constexpr auto min = 1e-4;
        const Vector3 op = position - ray.origin;
        const float b = dot(op, ray.direction);
        const float det = b * b - dot(op, op) + radius * radius;

        if (det < 0) {
            return std::nullopt;
        }
        const auto t1 = (b - std::sqrt(det));

        if (min <= t1 && t1 < tmax) {
            auto point = ray.origin + ray.direction * t1;
            auto normal = (point - position) / radius;
            return Hit{t1, point, normal, this};
        }

        const auto t2 = (b + std::sqrt(det));
        if (min <= t2 && t2 < tmax) {
            auto point = ray.origin + ray.direction * t2;
            auto normal = (point - position) / radius;
            return Hit{t2, point, normal, this};
        }
        return std::nullopt;
    }

    Sphere::Sphere(Vector3 p, float r, Material *m) : Object(m), position(p), radius(r)
    {

    }

    Aabb Sphere::getAABB() const
    {
        Vector3 min(position.x - radius, position.y - radius, position.z - radius);
        Vector3 max(position.x + radius, position.y + radius, position.z + radius);
        return Aabb(min, max);
    }

    Hit Sphere::pointSampling(Hit surfaceInfo)
    {
        return Hit(0, nagato::Vector3(), nagato::Vector3(), nullptr);
    }

    std::string Sphere::toString() const
    {
        return "[Shpere]material : " + material->typeName() +
               ", radius : " + std::to_string(radius) +
               ", point : " + position.toString();
    }
}
 