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

        explicit Vector3(float v = 0);

        Vector3(float x, float y, float z);

        float operator[](int i) const;

        inline Vector3 operator+(Vector3 b) const {
          return Vector3(x + b.x, y + b.y, z + b.z);
        }

        inline Vector3 operator+(float b) const {
          return Vector3(x + b, y + b, z + b);
        }

        inline Vector3 operator-(Vector3 b) const {
          return Vector3(x - b.x, y - b.y, z - b.z);
        }


        inline Vector3 operator*(Vector3 b) const {
          return Vector3(x * b.x, y * b.y, z * b.z);
        }

        inline Vector3 operator*(float a) const {
          return Vector3(x * a, y * a, z * a);
        }


        inline Vector3 operator/(Vector3 b) const {
          return Vector3(x / b.x, y / b.y, z / b.z);
        }

        inline Vector3 operator/(float b) const {
          return Vector3(x / b, y / b, z / b);
        }

        inline Vector3 operator-() const {
          return Vector3(-x, -y, -z);
        }

        friend inline bool operator==(Vector3 a, Vector3 b) {
          return a.x == b.x && a.y == b.y && a.z == b.z;
        }

        friend inline bool operator!=(Vector3 a, Vector3 b) {
          return a.x != b.x || a.y != b.y || a.z != b.z;
        }

        inline float norm() {
          return x * x + y * y + z * z;
        }

        inline std::string toString() const {
            char *str = nullptr;
            sprintf(str, "(%f, %f, %f)", x, y, z);
            return std::string(str);
        }
    };

    inline Vector4 toVec4(Vector3 v, float a = 1) {
      return Vector4(v.x, v.y, v.z, a);
    }


    inline Vector3 toVec3(Vector4 v) {
      return Vector3(v.x / v.w, v.y / v.w, v.z / v.w);
    }

    inline float dot(Vector3 a, Vector3 b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline Vector3 cross(Vector3 a, Vector3 b) {
      return Vector3(a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x);
    }

    inline Vector3 normalize(Vector3 v) {
      return v / sqrt(dot(v, v));
    }

    inline void printVector3(Vector3 v) {
      std::cout << v.x << " " << v.y << " " << v.z << std::endl;
    }
}

#endif //PATHTRACING_VECTOR3_HPP
