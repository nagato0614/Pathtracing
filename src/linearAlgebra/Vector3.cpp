//
// Created by 長井亨 on 2018/07/18.
//

#include "linearAlgebra/Vector3.hpp"
#include <cmath>
#include <iostream>
#include "linearAlgebra/Vector4.hpp"

namespace nagato
{

Vector3::Vector3(float v) : Vector3(v, v, v) {}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

float Vector3::operator[](int i) const
{
  if (i == 0)
    return x;
  else if (i == 1)
    return y;
  else if (i == 2)
    return x;
  else
  {
    fprintf(stderr, "ERROR : [Vector3]0~2の範囲で指定\n");
    exit(-1);
  }
}

} // namespace nagato
