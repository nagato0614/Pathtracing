//
// Created by 長井亨 on 2018/07/10.
//

#ifndef PATHTRACING_VECTOR4_HPP
#define PATHTRACING_VECTOR4_HPP

#include <cmath>

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#include "linearAlgebra/Matrix4.hpp"

namespace nagato
{
class Vector4
{
  public:
    float x;
    float y;
    float z;
    float w;

    explicit Vector4(float v = 0);

    Vector4(float x, float y, float z, float w);

    float operator[](int i);

    inline Vector4 operator+(Vector4 b) const
    {
#if defined(__ARM_NEON)
      Vector4 result;
      auto sum = vaddq_f32(vld1q_f32(&x), vld1q_f32(&b.x));
      vst1q_f32(&result.x, sum);
      return result;
#else
      return Vector4(x + b.x, y + b.y, z + b.z, w + b.w);
#endif
    }

    inline Vector4 operator-(Vector4 b) const
    {
#if defined(__ARM_NEON)
      Vector4 result;
      auto diff = vsubq_f32(vld1q_f32(&x), vld1q_f32(&b.x));
      vst1q_f32(&result.x, diff);
      return result;
#else
      return Vector4(x - b.x, y - b.y, z - b.z, w - b.w);
#endif
    }

    inline Vector4 operator*(Vector4 b) const
    {
#if defined(__ARM_NEON)
      Vector4 result;
      auto prod = vmulq_f32(vld1q_f32(&x), vld1q_f32(&b.x));
      vst1q_f32(&result.x, prod);
      return result;
#else
      return Vector4(x * b.x, y * b.y, z * b.z, w * b.w);
#endif
    }

    inline Vector4 operator*(float a) const
    {
#if defined(__ARM_NEON)
      Vector4 result;
      auto scaled = vmulq_n_f32(vld1q_f32(&x), a);
      vst1q_f32(&result.x, scaled);
      return result;
#else
      return Vector4(x * a, y * a, z * a, w * a);
#endif
    }

    friend inline Vector4 operator*(Vector4 a, Matrix4 b)
    {
#if defined(__ARM_NEON)
      Vector4 result(0);
      auto lhs = vld1q_f32(&a.x);

      for (int j = 0; j < 4; ++j)
      {
        float columnData[4] = {b.data[0][j], b.data[1][j], b.data[2][j], b.data[3][j]};
        auto mul = vmulq_f32(lhs, vld1q_f32(columnData));
        (&result.x)[j] = simd::horizontalAdd(mul);
      }
      return result;
#else
      Vector4 v(0);

      v.x = a.x * b.data[0][0] + a.y * b.data[1][0] + a.z * b.data[2][0] + a.w * b.data[3][0];
      v.y = a.x * b.data[0][1] + a.y * b.data[1][1] + a.z * b.data[2][1] + a.w * b.data[3][1];
      v.z = a.x * b.data[0][2] + a.y * b.data[1][2] + a.z * b.data[2][2] + a.w * b.data[3][2];
      v.w = a.x * b.data[0][3] + a.y * b.data[1][3] + a.z * b.data[2][3] + a.w * b.data[3][3];

      return v;
#endif
    }

    friend inline Vector4 operator*(Matrix4 a, Vector4 b)
    {
#if defined(__ARM_NEON)
      Vector4 result(0);
      auto rhs = vld1q_f32(&b.x);

      for (int i = 0; i < 4; ++i)
      {
        auto row = vld1q_f32(a.data[i]);
        auto mul = vmulq_f32(row, rhs);
        (&result.x)[i] = simd::horizontalAdd(mul);
      }
      return result;
#else
      Vector4 v(0);

      v.x = a.data[0][0] * b.x + a.data[0][1] * b.y + a.data[0][2] * b.z + a.data[0][3] * b.w;
      v.y = a.data[1][0] * b.x + a.data[1][1] * b.y + a.data[1][2] * b.z + a.data[1][3] * b.w;
      v.z = a.data[2][0] * b.x + a.data[2][1] * b.y + a.data[2][2] * b.z + a.data[2][3] * b.w;
      v.w = a.data[3][0] * b.x + a.data[3][1] * b.y + a.data[3][2] * b.z + a.data[3][3] * b.w;

      return v;
#endif
    }

    inline Vector4 operator/(Vector4 b) const
    {
#if defined(__ARM_NEON)
      Vector4 result;
      auto lhs = vld1q_f32(&x);
      auto rhs = vld1q_f32(&b.x);
      auto recip = vrecpeq_f32(rhs);
      recip = vmulq_f32(vrecpsq_f32(rhs, recip), recip);
      auto div = vmulq_f32(lhs, recip);
      vst1q_f32(&result.x, div);
      return result;
#else
      return Vector4(x / b.x, y / b.y, z / b.z, w / b.w);
#endif
    }

    inline Vector4 operator/(float b) const
    {
#if defined(__ARM_NEON)
      Vector4 result;
      auto scale = vmulq_n_f32(vld1q_f32(&x), 1.0f / b);
      vst1q_f32(&result.x, scale);
      return result;
#else
      return Vector4(x / b, y / b, z / b, w / b);
#endif
    }

    inline Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }
};

inline float dot(Vector4 a, Vector4 b)
{
#if defined(__ARM_NEON)
  auto prod = vmulq_f32(vld1q_f32(&a.x), vld1q_f32(&b.x));
  return simd::horizontalAdd(prod);
#else
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
#endif
}

//	inline Vector4 cross(Vector4 a, Vector4 b)
//	{
//		return Vector4(a.y * b.z - a.z * b.y,
//									 a.z * b.x - a.x * b.z,
//									 a.x * b.y - a.y * b.x);
//	}

inline Vector4 normalize(Vector4 v) { return v / std::sqrt(dot(v, v)); }

void printVector4(Vector4 a);
} // namespace nagato

#endif // PATHTRACING_VECTOR4_HPP
