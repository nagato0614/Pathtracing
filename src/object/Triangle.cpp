//
// Created by 長井亨 on 2018/07/28.
//

#include "Triangle.hpp"
#include <cmath>
#include <utility>

namespace nagato{
    Triangle::Triangle(Material *m, std::vector<Vector3> p) : Object(m), points(std::move(p))
    {

    }

    std::optional<Hit> Triangle::intersect(Ray &ray, float tmin, float tmax)
    {
        const auto normal = normalize(cross(points[1] - points[0], points[2] - points[1]));
        const auto origin = ray.origin + ray.direction * tmin;
        constexpr auto epsilon = 0.000001;

        const auto edge1 = points[1] - points[0];
        const auto edge2 = points[2] - points[0];

        const auto P = cross(ray.direction, edge2);
        const float det = dot(P, edge1);

        if (det > epsilon) {
            Vector3 T = origin - points[0];
            float u = dot(P, T);

            if (u >= 0.0 && u <= 1.0 * det) {
                const auto Q = cross(T, edge1);
                const auto v = dot(Q, ray.direction);

                if (v >= 0.0 && (u + v) <= 1.0 * det) {
                    float t = dot(Q, edge2) / det;
                    auto hitpoint = origin + ray.direction * t;
                    float distance = std::sqrt((hitpoint - origin).norm());
                    if (tmax > distance) {
                        return Hit{distance, hitpoint, normal, this};
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

        for (int i = 0; i < 3; i++) {
            if (min.x > points[i].x) {
                min.x = points[i].x;
            }
            if (min.y > points[i].y) {
                min.y = points[i].y;
            }
            if (min.z > points[i].z) {
                min.z = points[i].z;
            }

            if (max.x < points[i].x) {
                max.x = points[i].x;
            }
            if (max.y < points[i].y) {
                max.y = points[i].y;
            }
            if (max.z < points[i].z) {
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
        auto distance = (sampledPoint - surfaceInfo.point).norm();

        // サンプリング地点の法線ベクトル
        auto normal = normalize(cross(points[1] - points[0], points[2] - points[1]));

        return Hit{distance, sampledPoint, normal, this};
    }

    std::string Triangle::toString() const
    {
        return "[Triangle]material : " + material->typeName();
    }
}