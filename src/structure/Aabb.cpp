//
// Created by 長井亨 on 2018/07/28.
//

#include "Aabb.hpp"
namespace nagato{
    Aabb::Aabb(Vector3 min, Vector3 max) : min(min), max(max)
    {

    }

    bool Aabb::intersect(Ray &ray, float tmin, float tmax)
    {
        return false;
    }

    Aabb::Aabb() : min(Vector3(MAXFLOAT)), max(Vector3(-MAXFLOAT))
    {

    }
}