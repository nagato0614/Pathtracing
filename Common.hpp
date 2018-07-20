//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_COMMON_HPP
#define PATHTRACING_COMMON_HPP

#define RESOLUTION 400
#define SAMPLE 32

#include <tuple>
#include <vector>
#include "Vector3.hpp"

namespace nagato {

    std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n);

    int tonemap(double v);;

    int clamp(double v);

    std::vector<int> make_rand_array_unique(size_t size, int rand_min, int rand_max);

    std::string getNowTimeString();
}
#endif //PATHTRACING_COMMON_HPP
