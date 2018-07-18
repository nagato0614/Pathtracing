//
// Created by 長井亨 on 2018/07/18.
//

#include <iostream>
#include <cmath>
#include "Matrix4.hpp"

namespace nagato {

    Matrix4::Matrix4(double a) {
      for (auto &i : data)
        for (double &j : i)
          j = a;
    }

    Matrix4::Matrix4(double (*a)[4]) {
      for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
          data[i][j] = a[i][j];
    }

    void printMatrix4(Matrix4 a) {
      std::cout << "--------------------------------" << std::endl;
      for (auto &i : a.data) {
        std::cout << i[0] << " " << i[1] << " " << i[2] << " " << i[3] << std::endl;
      }
      std::cout << "--------------------------------" << std::endl;
    }

}