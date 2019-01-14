//
// Created by 長井亨 on 2018/07/18.
//

#include "Hit.hpp"

namespace nagato {
    Hit::Hit(float d, Vector3 p, Vector3 n, Object *sphere)
            :
            distance(d), point(p), normal(n), object(sphere) {

    }
}
