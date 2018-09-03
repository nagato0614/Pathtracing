//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_RAY_HPP
#define PATHTRACING_RAY_HPP


#include "../linearAlgebra/Vector3.hpp"

namespace nagato {
    class Ray {
    public:
        Vector3 origin;
        Vector3 direction;
    };
}

#endif //PATHTRACING_RAY_HPP
