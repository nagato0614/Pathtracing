//
// Created by 長井亨 on 2018/07/10.
//

#ifndef PATHTRACING_MATRIX4_HPP
#define PATHTRACING_MATRIX4_HPP

#include <cmath>

namespace nagato {
    class Matrix4 {
    public:
        float data[4][4] = {0};

        Matrix4()
        = default;

        explicit Matrix4(float a);

        explicit Matrix4(float a[4][4]);

        friend inline Matrix4 operator+(Matrix4 a, Matrix4 b) {
          Matrix4 m;
          for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
              m.data[i][j] = a.data[i][j] + b.data[i][j];
            }
          }
          return m;
        }


        friend inline Matrix4 operator-(Matrix4 a, Matrix4 b) {
          Matrix4 m;
          for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
              m.data[i][j] = a.data[i][j] - b.data[i][j];
            }
          }
          return m;
        }

        friend inline Matrix4 operator*(Matrix4 a, Matrix4 b) {
          Matrix4 m;

          for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
              for (int k = 0; k < 4; k++) {
                m.data[i][j] += a.data[i][k] * b.data[k][j];
              }
            }
          }
          return m;
        }


        inline static Matrix4 identity() {
          Matrix4 m;

          for (int i = 0; i < 4; i++)
            m.data[i][i] = 1;

          return m;
        }

        inline static Matrix4 transform(float x, float y, float z) {
          Matrix4 m = identity();
          m.data[0][3] = x;
          m.data[1][3] = y;
          m.data[2][3] = z;
          return m;
        }

        inline static Matrix4 rotate(int a, float theta) {
          Matrix4 m = identity();

          if (a == 0) {
            // x軸を中心に回転
            m.data[1][1] = static_cast<float>(cos(theta));
            m.data[1][2] = static_cast<float>(sin(theta));
            m.data[2][1] = static_cast<float>(-sin(theta));
            m.data[2][2] = static_cast<float>(cos(theta));
          } else if (a == 1) {
            // y軸を中心に回転
            m.data[0][0] = static_cast<float>(cos(theta));
            m.data[0][2] = static_cast<float>(-sin(theta));
            m.data[2][0] = static_cast<float>(sin(theta));
            m.data[2][2] = static_cast<float>(cos(theta));
          } else if (a == 2) {
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
}

#endif //PATHTRACING_MATRIX4_HPP
