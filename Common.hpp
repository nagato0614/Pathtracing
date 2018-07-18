//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_COMMON_HPP
#define PATHTRACING_COMMON_HPP


#include <tuple>
#include "Vector3.hpp"

namespace nagato {

    std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n);

    int tonemap(double v);;

    int clamp(double v);
}
#endif //PATHTRACING_COMMON_HPP
