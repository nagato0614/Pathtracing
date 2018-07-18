//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_HIT_HPP
#define PATHTRACING_HIT_HPP

#include "Object.hpp"
#include "Vector3.hpp"

namespace nagato {
    class Object;

    class Hit {
    public:
        double distance;
        Vector3 point;
        Vector3 normal;
        Object *sphere;

        Hit(double d, Vector3 p, Vector3 n, Object *sphere);
    };
}

#endif //PATHTRACING_HIT_HPP
