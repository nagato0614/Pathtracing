//
// Created by 長井亨 on 2018/07/17.
//

#ifndef PATHTRACING_MATRIX3_HPP
#define PATHTRACING_MATRIX3_HPP

#include <cmath>
#include <iostream>
#include "Vector3.hpp"

namespace nagato {
    class Matrix3 {
    public:
        double data[3][3] = {0};

        Matrix3()
        = default;

        explicit Matrix3(double a);

        explicit Matrix3(double a[3][3]);

        Matrix3(Vector3 a, Vector3 b, Vector3 c);

        friend inline Matrix3 operator+(Matrix3 a, Matrix3 b) {
          Matrix3 m;
          for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
              m.data[i][j] = a.data[i][j] + b.data[i][j];
            }
          }
          return m;
        }

        friend inline Matrix3 operator-(Matrix3 a, Matrix3 b) {
          Matrix3 m;
          for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
              m.data[i][j] = a.data[i][j] - b.data[i][j];
            }
          }
          return m;
        }

        friend inline Matrix3 operator*(Matrix3 a, Matrix3 b) {
          Matrix3 m;

          for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
              for (int k = 0; k < 3; k++) {
                m.data[i][j] += a.data[i][k] * b.data[k][j];
              }
            }
          }
          return m;
        }

        inline static Matrix3 identity() {
          Matrix3 m;

          for (int i = 0; i < 3; i++)
            m.data[i][i] = 1;

          return m;
        }

        inline double det() {
          double sum = 0;
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
}


#endif //PATHTRACING_MATRIX3_HPP
