//
// Created by 長井亨 on 2018/07/18.
//

#include <cmath>
#include <iostream>
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace nagato {

    Vector3::Vector3(float v)
            : Vector3(v, v, v) {}

    Vector3::Vector3(float x, float y, float z)
            : x(x), y(y), z(z) {}

    float Vector3::operator[](int i) {
      return (&x)[i];
    }


}
