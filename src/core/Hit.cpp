//
// Created by 長井亨 on 2018/07/18.
//

#include "Hit.hpp"

namespace nagato {
    Hit::Hit(float d, Vector3 p, Vector3 n, Object *sphere)
            :
            distance(d), point(p), normal(n), object(sphere) {

    }

    Hit::Hit() {

    }

    float Hit::getDistance() const {
        return distance;
    }

    Vector3 Hit::getPoint() const {
        return point;
    }

    Vector3 Hit::getNormal() const {
        return normal;
    }

    Object *Hit::getObject() const {
        return object;
    }
}
