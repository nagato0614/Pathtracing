//
// Created by 長井亨 on 2018/07/17.
//

#ifndef PATHTRACING_MATRIX3_HPP
#define PATHTRACING_MATRIX3_HPP

#include <cmath>
#include <iostream>

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif
#include "linearAlgebra/Vector3.hpp"

namespace nagato
{
class Matrix3
{
  public:
    float data[3][3] = {};

    Matrix3() = default;

    explicit Matrix3(float a);

    explicit Matrix3(float a[3][3]);

    Matrix3(Vector3 a, Vector3 b, Vector3 c);

    friend inline Matrix3 operator+(Matrix3 a, Matrix3 b)
    {
      Matrix3 m;
#if defined(__ARM_NEON)
      for (int i = 0; i < 3; i++)
      {
        auto lhs = vld1_f32(&a.data[i][0]);
        auto rhs = vld1_f32(&b.data[i][0]);
        auto sum = vadd_f32(lhs, rhs);
        vst1_f32(&m.data[i][0], sum);
        m.data[i][2] = a.data[i][2] + b.data[i][2];
      }
#else
      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          m.data[i][j] = a.data[i][j] + b.data[i][j];
        }
      }
#endif
      return m;
    }

    friend inline Matrix3 operator-(Matrix3 a, Matrix3 b)
    {
      Matrix3 m;
#if defined(__ARM_NEON)
      for (int i = 0; i < 3; i++)
      {
        auto lhs = vld1_f32(&a.data[i][0]);
        auto rhs = vld1_f32(&b.data[i][0]);
        auto diff = vsub_f32(lhs, rhs);
        vst1_f32(&m.data[i][0], diff);
        m.data[i][2] = a.data[i][2] - b.data[i][2];
      }
#else
      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          m.data[i][j] = a.data[i][j] - b.data[i][j];
        }
      }
#endif
      return m;
    }

    friend inline Matrix3 operator*(Matrix3 a, Matrix3 b)
    {
      Matrix3 m;

      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          for (int k = 0; k < 3; k++)
          {
            m.data[i][j] += a.data[i][k] * b.data[k][j];
          }
        }
      }
      return m;
    }

    friend inline Vector3 operator*(Matrix3 a, Vector3 b)
    {
      Vector3 v;

#if defined(__ARM_NEON)
      auto bxy = vld1_f32(&b.x);

      auto row0 = vld1_f32(&a.data[0][0]);
      auto row1 = vld1_f32(&a.data[1][0]);
      auto row2 = vld1_f32(&a.data[2][0]);

      auto prod0 = vmul_f32(row0, bxy);
      auto prod1 = vmul_f32(row1, bxy);
      auto prod2 = vmul_f32(row2, bxy);

      auto sum0 = vpadd_f32(prod0, prod0);
      auto sum1 = vpadd_f32(prod1, prod1);
      auto sum2 = vpadd_f32(prod2, prod2);

      v.x = vget_lane_f32(sum0, 0) + a.data[0][2] * b.z;
      v.y = vget_lane_f32(sum1, 0) + a.data[1][2] * b.z;
      v.z = vget_lane_f32(sum2, 0) + a.data[2][2] * b.z;
#else
      v.x = b.x * a.data[0][0] + b.y * a.data[0][1] + b.z * a.data[0][2];
      v.y = b.x * a.data[1][0] + b.y * a.data[1][1] + b.z * a.data[1][2];
      v.z = b.x * a.data[2][0] + b.y * a.data[2][1] + b.z * a.data[2][2];
#endif

      return v;
    }

    inline static Matrix3 identity()
    {
      Matrix3 m;

      for (int i = 0; i < 3; i++)
        m.data[i][i] = 1;

      return m;
    }

    inline float det()
    {
      float sum = 0;
      sum += data[0][0] * data[1][1] * data[2][2];
      sum += data[0][1] * data[1][2] * data[2][0];
      sum += data[0][2] * data[1][0] * data[2][1];
      sum -= data[0][2] * data[1][1] * data[2][0];
      sum -= data[0][1] * data[1][2] * data[2][0];
      sum -= data[0][0] * data[1][2] * data[2][1];
      return sum;
    }
};

void printMatrix3(Matrix3 a);
} // namespace nagato

#endif // PATHTRACING_MATRIX3_HPP
