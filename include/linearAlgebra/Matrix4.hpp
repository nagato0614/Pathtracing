//
// Created by 長井亨 on 2018/07/10.
//

#ifndef PATHTRACING_MATRIX4_HPP
#define PATHTRACING_MATRIX4_HPP

#include <cmath>

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

namespace nagato
{
#if defined(__ARM_NEON)
namespace simd
{
inline float horizontalAdd(float32x4_t v)
{
#if defined(__aarch64__)
  return vaddvq_f32(v);
#else
  auto sum = vadd_f32(vget_low_f32(v), vget_high_f32(v));
  sum = vpadd_f32(sum, sum);
  return vget_lane_f32(sum, 0);
#endif
}
} // namespace simd
#endif

class Matrix4
{
  public:
    float data[4][4] = {};

    Matrix4() = default;

    explicit Matrix4(float a);

    explicit Matrix4(float a[4][4]);

    friend inline Matrix4 operator+(Matrix4 a, Matrix4 b)
    {
      Matrix4 m;
#if defined(__ARM_NEON)
      for (int i = 0; i < 4; i++)
      {
        auto rowA = vld1q_f32(a.data[i]);
        auto rowB = vld1q_f32(b.data[i]);
        auto sum = vaddq_f32(rowA, rowB);
        vst1q_f32(m.data[i], sum);
      }
#else
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          m.data[i][j] = a.data[i][j] + b.data[i][j];
        }
      }
#endif
      return m;
    }

    friend inline Matrix4 operator-(Matrix4 a, Matrix4 b)
    {
      Matrix4 m;
#if defined(__ARM_NEON)
      for (int i = 0; i < 4; i++)
      {
        auto rowA = vld1q_f32(a.data[i]);
        auto rowB = vld1q_f32(b.data[i]);
        auto diff = vsubq_f32(rowA, rowB);
        vst1q_f32(m.data[i], diff);
      }
#else
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          m.data[i][j] = a.data[i][j] - b.data[i][j];
        }
      }
#endif
      return m;
    }

    friend inline Matrix4 operator*(Matrix4 a, Matrix4 b)
    {
      Matrix4 m;

#if defined(__ARM_NEON)
      float32x4_t rowsA[4];
      for (int i = 0; i < 4; ++i)
        rowsA[i] = vld1q_f32(a.data[i]);

      for (int j = 0; j < 4; ++j)
      {
        float columnData[4] = {b.data[0][j], b.data[1][j], b.data[2][j], b.data[3][j]};
        auto column = vld1q_f32(columnData);
        for (int i = 0; i < 4; ++i)
        {
          m.data[i][j] = simd::horizontalAdd(vmulq_f32(rowsA[i], column));
        }
      }
#else
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          for (int k = 0; k < 4; k++)
          {
            m.data[i][j] += a.data[i][k] * b.data[k][j];
          }
        }
      }
#endif
      return m;
    }

    inline static Matrix4 identity()
    {
      Matrix4 m;

      for (int i = 0; i < 4; i++)
        m.data[i][i] = 1;

      return m;
    }

    inline static Matrix4 transform(float x, float y, float z)
    {
      Matrix4 m = identity();
      m.data[0][3] = x;
      m.data[1][3] = y;
      m.data[2][3] = z;
      return m;
    }

    inline static Matrix4 rotate(int a, float theta)
    {
      Matrix4 m = identity();

      if (a == 0)
      {
        // x軸を中心に回転
        m.data[1][1] = static_cast<float>(cos(theta));
        m.data[1][2] = static_cast<float>(sin(theta));
        m.data[2][1] = static_cast<float>(-sin(theta));
        m.data[2][2] = static_cast<float>(cos(theta));
      }
      else if (a == 1)
      {
        // y軸を中心に回転
        m.data[0][0] = static_cast<float>(cos(theta));
        m.data[0][2] = static_cast<float>(-sin(theta));
        m.data[2][0] = static_cast<float>(sin(theta));
        m.data[2][2] = static_cast<float>(cos(theta));
      }
      else if (a == 2)
      {
        // z軸を中心に回転
        m.data[0][0] = static_cast<float>(cos(theta));
        m.data[0][1] = static_cast<float>(sin(theta));
        m.data[1][0] = static_cast<float>(-sin(theta));
        m.data[1][1] = static_cast<float>(cos(theta));
      }
      return m;
    }
};

void printMatrix4(Matrix4 a);
} // namespace nagato

#endif // PATHTRACING_MATRIX4_HPP
