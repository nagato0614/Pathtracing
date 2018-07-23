//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_HIT_HPP
#define PATHTRACING_HIT_HPP

#include "Object.hpp"

namespace nagato {
    class Object;

    class Hit {
    public:
        float distance;
        Vector3 point;
        Vector3 normal;
        Object *sphere;

        Hit(float d, Vector3 p, Vector3 n, Object *sphere);
    };
}

#endif //PATHTRACING_HIT_HPP
