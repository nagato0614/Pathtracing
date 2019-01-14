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
        auto z = Random::Instance().nextFloat(-1.f, 1.f);
        auto phi = Random::Instance().nextFloat(0, 2.0f * M_PI);

        auto x = std::sqrt(1.0f - std::pow(z, 2.0f)) * std::cos(phi);
        auto y = std::sqrt(1.0f - std::pow(z, 2.0f)) * std::sin(phi);

        Vector3 sampledPoint{x, y, z};
        sampledPoint = position + sampledPoint * radius;
        auto normal = sampledPoint;
        auto distance = std::sqrt((sampledPoint - surfaceInfo.point).norm());

        return Hit{distance, sampledPoint, normal, this};
    }

    std::string Sphere::toString() const
    {
        return "[Shpere]material : " + material->typeName() +
               ", radius : " + std::to_string(radius) +
               ", point : " + position.toString();
    }

    float Sphere::area() const {
        return 4.0f * M_PI * std::pow(radius, 2.0f);
    }
}
 