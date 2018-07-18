//
// Created by 長井亨 on 2018/07/18.
//

#include <cmath>
#include <iostream>
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace nagato {

    Vector3::Vector3(double v)
            : Vector3(v, v, v) {}

    Vector3::Vector3(double x, double y, double z)
            : x(x), y(y), z(z) {}

    double Vector3::operator[](int i) {
      return (&x)[i];
    }


}