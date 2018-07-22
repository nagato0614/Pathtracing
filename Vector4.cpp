//
// Created by 長井亨 on 2018/07/18.
//

#include <cmath>
#include <iostream>
#include "Vector4.hpp"

namespace nagato {

    Vector4::Vector4(float v)
            : Vector4(v, v, v, v) {}

    Vector4::Vector4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w) {}

    float Vector4::operator[](int i) {
      return (&x)[i];
    }

    void printVector4(Vector4 a) {
      std::cout << "--------------------------------" << std::endl;
      std::cout << a.x << " " << a.y << " " << a.z << " " << a.w << std::endl;
      std::cout << "--------------------------------" << std::endl;
    }

}
