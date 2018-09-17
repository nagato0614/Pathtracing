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
        float data[3][3] = {};

        Matrix3()
        = default;

        explicit Matrix3(float a);

        explicit Matrix3(float a[3][3]);

        Matrix3(Vector3 a, Vector3 b, Vector3 c);

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

        friend inline Vector3 operator*(Matrix3 a, Vector3 b)
        {
            Vector3 v;

            v.x = b.x * a.data[0][0] + b.y * a.data[0][1] + b.z * a.data[0][2];
            v.y = b.x * a.data[1][0] + b.y * a.data[1][1] + b.z * a.data[1][2];
            v.z = b.x * a.data[2][0] + b.y * a.data[2][1] + b.z * a.data[2][2];

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
}


#endif //PATHTRACING_MATRIX3_HPP
