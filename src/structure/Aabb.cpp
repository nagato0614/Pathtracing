//
// Created by 長井亨 on 2018/07/28.
//

#include <float.h>
#include "Aabb.hpp"
namespace nagato{
    Aabb::Aabb(Vector3 min, Vector3 max) : min(min), max(max)
    {

    }

    bool Aabb::intersect(Ray &ray)
    {
        float t_max =  FLT_MAX ; // AABB からレイが外に出る時刻
        float t_min = -FLT_MAX;  // AABB にレイが侵入する時刻

        for (int i=0; i<3; i++) {
            float t1 = (min[i] - ray.origin[i])/ray.direction[i];
            float t2 = (max[i] - ray.origin[i])/ray.direction[i];
            float t_near = std::min(t1, t2);
            float t_far = std::max(t1, t2);
            t_max = std::min(t_max, t_far);
            t_min = std::max(t_min, t_near);

            // レイが外に出る時刻と侵入する時刻が逆転している => 交差していない
            if (t_min > t_max) return false;
        }
        return true;
    }

    Aabb::Aabb() : min(Vector3(MAXFLOAT)), max(Vector3(MAXFLOAT))
    {

    }

    Vector3 Aabb::getCenter()
    {
        return Vector3{static_cast<float>((min.x + max.x) / 2.0),
                       static_cast<float>((min.y + max.y) / 2.0),
                       static_cast<float>((min.z + max.z) / 2.0)};
    }
}