//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_AABB_HPP
#define PATHTRACING_AABB_HPP

#include "Vector3.hpp"

namespace nagato
{
    class Aabb
    {
     public:
        Vector3 min;
        Vector3 max;

    };
}

#endif //PATHTRACING_AABB_HPP
