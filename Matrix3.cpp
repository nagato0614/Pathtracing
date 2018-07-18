//
// Created by 長井亨 on 2018/07/18.
//

#include "Matrix3.hpp"

namespace nagato {

    Matrix3::Matrix3(double a) {
      for (auto &i : data)
        for (double &j : i)
          j = a;
    }

    Matrix3::Matrix3(double (*a)[3]) {
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          data[i][j] = a[i][j];
    }

    Matrix3::Matrix3(Vector3 a, Vector3 b, Vector3 c) {
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


    void printMatrix3(Matrix3 a) {
      std::cout << "--------------------------------" << std::endl;
      for (auto &i : a.data) {
        std::cout << i[0] << " " << i[1] << " " << i[2] << std::endl;
      }
      std::cout << "--------------------------------" << std::endl;
    }
}