//
// Created by Codex on 2024/10/08.
//

#ifndef PATHTRACING_VECTOR2_HPP
#define PATHTRACING_VECTOR2_HPP

#include <cmath>
#include <string>

namespace nagato
{
class Vector2
{
  public:
    float x;
    float y;

    explicit Vector2(float v = 0.0f) : x(v), y(v) {}

    Vector2(float x, float y) : x(x), y(y) {}

    float operator[](int i) const { return i == 0 ? x : y; }

    friend inline bool operator==(const Vector2 &a, const Vector2 &b)
    {
      return a.x == b.x && a.y == b.y;
    }

    friend inline bool operator!=(const Vector2 &a, const Vector2 &b)
    {
      return !(a == b);
    }

    inline std::string toString() const
    {
      return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

inline Vector2 operator+(const Vector2 &a, const Vector2 &b)
{
  return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator-(const Vector2 &a, const Vector2 &b)
{
  return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator*(const Vector2 &a, float b)
{
  return Vector2(a.x * b, a.y * b);
}

inline Vector2 operator*(float a, const Vector2 &b)
{
  return Vector2(a * b.x, a * b.y);
}

inline Vector2 operator/(const Vector2 &a, float b)
{
  return Vector2(a.x / b, a.y / b);
}

inline Vector2 lerp(const Vector2 &a, const Vector2 &b, float t)
{
  return a * (1.0f - t) + b * t;
}
} // namespace nagato

#endif // PATHTRACING_VECTOR2_HPP
