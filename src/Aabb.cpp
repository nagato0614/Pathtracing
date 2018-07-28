//
// Created by 長井亨 on 2018/07/28.
//

#include "Aabb.hpp"
namespace nagato{
    Aabb::Aabb(Vector3 m1, Vector3 m2) : min(m1), min(m2)
    {

    }

    bool Aabb::intersect(Ray &ray, float tmin, float tmax)
    {
        return false;
    }
}