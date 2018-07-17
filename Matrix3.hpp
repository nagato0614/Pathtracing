//
// Created by 長井亨 on 2018/07/17.
//

#ifndef PATHTRACING_MATRIX3_HPP
#define PATHTRACING_MATRIX3_HPP

#include <cmath>
#include <iostream>
#include "Vector3.hpp"

namespace nagato
{
    class Matrix3
    {
    public:
        double data[3][3] = {0};

        Matrix3()
        = default;

        explicit Matrix3(double a)
        {
          for (auto &i : data)
            for (double &j : i)
              j = a;
        }

        explicit Matrix3(double a[3][3])
        {
          for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
              data[i][j] = a[i][j];
        }

        Matrix3(Vector3 a, Vector3 b, Vector3 c)
        {
          data[0][0] = a.x;
          data[0][1] = a.y;
          data[0][2] = a.z;
          data[1][0] = b.x;
          data[1][1] = b.y;
          data[1][2] = b.z;
          data[2][0] = c.x;
          data[2][1] = c.y;
          data[2][2] = c.z;
        }

        friend inline Matrix3 operator+(Matrix3 a, Matrix3 b)
        {
          Matrix3 m;
          for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
              m.data[i][j] = a.data[i][j] + b.data[i][j];
            }
          }
          return m;
        }

        friend inline Matrix3 operator-(Matrix3 a, Matrix3 b)
        {
          Matrix3 m;
          for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
              m.data[i][j] = a.data[i][j] - b.data[i][j];
            }
          }
          return m;
        }

        friend inline Matrix3 operator*(Matrix3 a, Matrix3 b)
        {
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

        inline static Matrix3 identity()
        {
          Matrix3 m;

          for (int i = 0; i < 3; i++)
            m.data[i][i] = 1;

          return m;
        }

        inline double det()
        {
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

    void printMatrix3(Matrix3 a)
    {
      std::cout << "--------------------------------" << std::endl;
      for (auto &i : a.data) {
        std::cout << i[0] << " " << i[1] << " " << i[2] << std::endl;
      }
      std::cout << "--------------------------------" << std::endl;
    }
}



#endif //PATHTRACING_MATRIX3_HPP
