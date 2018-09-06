//
// Created by 長井亨 on 2018/07/28.
//

#include "Aabb.hpp"
namespace nagato{
    Aabb::Aabb(Vector3 min, Vector3 max) : min(min), max(max)
    {

    }

    bool Aabb::intersect(Ray &ray)
    {
        int h = 2;
        for (int v = 0; v < 3; v++) {
            if (min[v] * ray.direction[h] > max[h] * ray.direction[v] ||
                    min[h] * ray.direction[v] > max[v] * ray.direction[h])
                return false;

            h = v;
        }
        return true;
    }

    Aabb::Aabb() : min(Vector3(MAXFLOAT)), max(Vector3(-MAXFLOAT))
    {

    }

    Vector3 Aabb::getCenter()
    {
        return Vector3{static_cast<float>((min.x + max.x) / 2.0),
                       static_cast<float>((min.y + max.y) / 2.0),
                       static_cast<float>((min.z + max.z) / 2.0)};
    }
}