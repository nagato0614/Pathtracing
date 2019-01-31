//
// Created by 長井亨 on 2019-01-31.
//

#ifndef PATHTRACING_LINEARALGEBRA_HPP
#define PATHTRACING_LINEARALGEBRA_HPP

#include "Vector3.hpp"

namespace nagato{
    std::tuple<Vector3, Vector3, Vector3> orthonormalBasis(const Vector3 &v1);

    Vector3 worldToLocal(const Vector3 &v, const Vector3 &normal);

    Vector3 localToWorld(const Vector3 &v, const Vector3 &normal);
}

#endif //PATHTRACING_LINEARALGEBRA_HPP
