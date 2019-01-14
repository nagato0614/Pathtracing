//
// Created by kiki on 2018/06/26.
//

#ifndef PATHTRACING_VECTOR3_HPP
#define PATHTRACING_VECTOR3_HPP

#include <iostream>
#include "Vector4.hpp"

namespace nagato {
    class Vector3 {
    public:
        float x;
        float y;
        float z;

        explicit Vector3(float v = 0) ;

        Vector3(float x, float y, float z);

        float operator[](int i) const;

        friend inline bool operator==(const Vector3 &a, const Vector3 &b) {
          return a.x == b.x && a.y == b.y && a.z == b.z;
        }

        friend inline bool operator!=(const Vector3 &a, const Vector3 &b) {
          return a.x != b.x || a.y != b.y || a.z != b.z;
        }

        inline float norm() {
          return x * x + y * y + z * z;
        }

        inline std::string toString() const {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
        }
    };

    inline Vector3 operator+(const Vector3 &a , const Vector3 &b) {
        return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    inline Vector3 operator+(const Vector3 &a, float b) {
        return Vector3(a.x + b, a.y + b, a.z + b);
    }

    inline Vector3 operator+(float a, const Vector3 &b) {
        return Vector3(a + b.x, a + b.y, a + b.z);
    }

    inline Vector3 operator-(const Vector3 &a, const Vector3 &b) {
        return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    inline Vector3 operator-(const Vector3 &a, float b) {
        return Vector3(a.x - b, a.y - b, a.z - b);
    }

    inline Vector3 operator-(float a, const Vector3 &b) {
        return Vector3(a - b.x, a - b.y, a - b.z);
    }

    inline Vector3 operator*(const Vector3 &a, const Vector3 &b) {
        return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
    }

    inline Vector3 operator*(const Vector3 &a, float b) {
        return Vector3(a.x * b, a.y * b, a.z * b);
    }

    inline Vector3 operator*(float a, const Vector3 &b) {
        return Vector3(a * b.x, a * b.x, a * b.x);
    }

    inline Vector3 operator/(const Vector3 &a, const Vector3 &b) {
        return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
    }

    inline Vector3 operator/(float a, const Vector3 &b) {
        return Vector3(a / b.x, a / b.y, a / b.z);
    }

    inline Vector3 operator/(const Vector3 &a, float b) {
        return Vector3(a.x / b, a.y / b, a.z / b);
    }

    inline Vector3 operator-(const Vector3 &a)  {
        return Vector3(-a.x, -a.y, -a.z);
    }

    inline Vector4 toVec4(const Vector3 &v, float a = 1) {
      return Vector4(v.x, v.y, v.z, a);
    }


    inline Vector3 toVec3(const Vector4 &v) {
      return Vector3(v.x / v.w, v.y / v.w, v.z / v.w);
    }

    inline float dot(const Vector3 &a, const Vector3 &b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline Vector3 cross(const Vector3 &a, const Vector3 &b) {
      return Vector3(a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x);
    }

    inline Vector3 normalize(const Vector3 &v) {
      return v / std::sqrt(dot(v, v));
    }

    inline void printVector3(const Vector3 &v) {
      std::cout << v.x << " " << v.y << " " << v.z << std::endl;
    }
}

#endif //PATHTRACING_VECTOR3_HPP
