//
// Created by 長井亨 on 2018/06/26.
//

#ifndef PATHTRACING_VECTOR3_HPP
#define PATHTRACING_VECTOR3_HPP

#include <cmath>
#include <iostream>

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#include "linearAlgebra/Vector4.hpp"

namespace nagato
{
class Vector3
{
  public:
    float x;
    float y;
    float z;

    explicit Vector3(float v = 0);

    Vector3(float x, float y, float z);

    float operator[](int i) const;

    friend inline bool operator==(const Vector3 &a, const Vector3 &b)
    {
      return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    friend inline bool operator!=(const Vector3 &a, const Vector3 &b)
    {
      return a.x != b.x || a.y != b.y || a.z != b.z;
    }

    inline float norm() { return x * x + y * y + z * z; }

    inline std::string toString() const
    {
      return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }

inline float sum() const { return x + y + z; }

inline float max() const { return (x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z); }

inline void normlizeMaxValue()
{
  auto m = this->max();
  x /= m;
  y /= m;
  z /= m;
}
};

#if defined(__ARM_NEON)
namespace simd
{
inline float32x2_t loadXY(const Vector3 &v)
{
  return vld1_f32(&v.x);
}

inline void storeXY(Vector3 &dst, float32x2_t xy)
{
  vst1_f32(&dst.x, xy);
}
} // namespace simd
#endif

inline Vector3 operator+(const Vector3 &a, const Vector3 &b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto sumXY = vadd_f32(simd::loadXY(a), simd::loadXY(b));
  simd::storeXY(result, sumXY);
#else
  result.x = a.x + b.x;
  result.y = a.y + b.y;
#endif
  result.z = a.z + b.z;
  return result;
}

inline Vector3 operator+(const Vector3 &a, float b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto sumXY = vadd_f32(simd::loadXY(a), vdup_n_f32(b));
  simd::storeXY(result, sumXY);
#else
  result.x = a.x + b;
  result.y = a.y + b;
#endif
  result.z = a.z + b;
  return result;
}

inline Vector3 operator+(float a, const Vector3 &b) { return b + a; }

inline Vector3 operator-(const Vector3 &a, const Vector3 &b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto diffXY = vsub_f32(simd::loadXY(a), simd::loadXY(b));
  simd::storeXY(result, diffXY);
#else
  result.x = a.x - b.x;
  result.y = a.y - b.y;
#endif
  result.z = a.z - b.z;
  return result;
}

inline Vector3 operator-(const Vector3 &a, float b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto diffXY = vsub_f32(simd::loadXY(a), vdup_n_f32(b));
  simd::storeXY(result, diffXY);
#else
  result.x = a.x - b;
  result.y = a.y - b;
#endif
  result.z = a.z - b;
  return result;
}

inline Vector3 operator-(float a, const Vector3 &b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto diffXY = vsub_f32(vdup_n_f32(a), simd::loadXY(b));
  simd::storeXY(result, diffXY);
#else
  result.x = a - b.x;
  result.y = a - b.y;
#endif
  result.z = a - b.z;
  return result;
}

inline Vector3 operator*(const Vector3 &a, const Vector3 &b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto mulXY = vmul_f32(simd::loadXY(a), simd::loadXY(b));
  simd::storeXY(result, mulXY);
#else
  result.x = a.x * b.x;
  result.y = a.y * b.y;
#endif
  result.z = a.z * b.z;
  return result;
}

inline Vector3 operator*(const Vector3 &a, float b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto mulXY = vmul_f32(simd::loadXY(a), vdup_n_f32(b));
  simd::storeXY(result, mulXY);
#else
  result.x = a.x * b;
  result.y = a.y * b;
#endif
  result.z = a.z * b;
  return result;
}

inline Vector3 operator*(float a, const Vector3 &b) { return b * a; }

inline Vector3 operator/(const Vector3 &a, const Vector3 &b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto denomXY = simd::loadXY(b);
  auto recipXY = vrecpe_f32(denomXY);
  recipXY = vmul_f32(vrecps_f32(denomXY, recipXY), recipXY);
  auto divXY = vmul_f32(simd::loadXY(a), recipXY);
  simd::storeXY(result, divXY);
#else
  result.x = a.x / b.x;
  result.y = a.y / b.y;
#endif
  result.z = a.z / b.z;
  return result;
}

inline Vector3 operator/(float a, const Vector3 &b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto denomXY = simd::loadXY(b);
  auto recipXY = vrecpe_f32(denomXY);
  recipXY = vmul_f32(vrecps_f32(denomXY, recipXY), recipXY);
  auto divXY = vmul_f32(vdup_n_f32(a), recipXY);
  simd::storeXY(result, divXY);
#else
  result.x = a / b.x;
  result.y = a / b.y;
#endif
  result.z = a / b.z;
  return result;
}

inline Vector3 operator/(const Vector3 &a, float b)
{
  Vector3 result;
#if defined(__ARM_NEON)
  auto scale = vdup_n_f32(1.0f / b);
  auto divXY = vmul_f32(simd::loadXY(a), scale);
  simd::storeXY(result, divXY);
#else
  result.x = a.x / b;
  result.y = a.y / b;
#endif
  result.z = a.z / b;
  return result;
}

inline Vector3 operator-(const Vector3 &a) { return Vector3(-a.x, -a.y, -a.z); }

inline Vector4 toVec4(const Vector3 &v, float a = 1) { return Vector4(v.x, v.y, v.z, a); }

inline Vector3 toVec3(const Vector4 &v) { return Vector3(v.x / v.w, v.y / v.w, v.z / v.w); }

inline float dot(const Vector3 &a, const Vector3 &b)
{
#if defined(__ARM_NEON)
  auto prodXY = vmul_f32(simd::loadXY(a), simd::loadXY(b));
  auto sumXY = vpadd_f32(prodXY, prodXY);
  return vget_lane_f32(sumXY, 0) + a.z * b.z;
#else
  return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

inline Vector3 cross(const Vector3 &a, const Vector3 &b)
{
#if defined(__ARM_NEON)
  Vector3 result;

  auto ayz = vld1_f32(&a.y);
  auto bzy = vdup_n_f32(b.z);
  bzy = vset_lane_f32(b.y, bzy, 1);
  auto prodX = vmul_f32(ayz, bzy);
  result.x = vget_lane_f32(prodX, 0) - vget_lane_f32(prodX, 1);

  auto azx = vdup_n_f32(a.z);
  azx = vset_lane_f32(a.x, azx, 1);
  auto bxz = vdup_n_f32(b.x);
  bxz = vset_lane_f32(b.z, bxz, 1);
  auto prodY = vmul_f32(azx, bxz);
  result.y = vget_lane_f32(prodY, 0) - vget_lane_f32(prodY, 1);

  auto axy = vld1_f32(&a.x);
  auto byx = vdup_n_f32(b.y);
  byx = vset_lane_f32(b.x, byx, 1);
  auto prodZ = vmul_f32(axy, byx);
  result.z = vget_lane_f32(prodZ, 0) - vget_lane_f32(prodZ, 1);

  return result;
#else
  return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
#endif
}

inline Vector3 normalize(const Vector3 &v) { return v / std::sqrt(dot(v, v)); }

inline void printVector3(const Vector3 &v)
{
  std::cout << v.x << " " << v.y << " " << v.z << std::endl;
}
} // namespace nagato

#endif // PATHTRACING_VECTOR3_HPP
