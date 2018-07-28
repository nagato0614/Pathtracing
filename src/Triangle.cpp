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
        constexpr auto epsilon = 0.00000001;

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

                if (v >= 0.0 && (u + v) <= 1 * det) {
                    float t = dot(Q, edge2) / det;
                    auto hitpoint = origin + ray.direction * t;
                    float distance = std::sqrt((hitpoint - origin).norm());
                    if (tmax > distance) {
                        return Hit{distance, hitpoint, normal, this};
                    }
                }
            }
        }
    }
}