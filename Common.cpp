//
// Created by 長井亨 on 2018/07/18.
//

#include <cmath>
#include "Common.hpp"

namespace nagato {

    std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n) {
      const double s = std::copysign(1, n.z);
      const double a = -1 / (s + n.z);
      const double b = n.x * n.y * a;
      return {
              Vector3(1 + s * n.x * n.x * a, s * b, -s * n.x),
              Vector3(b, s + n.y * n.y * a, -n.y)
      };
    }

    int tonemap(double v) {
      return std::min(
              std::max(int(std::pow(v, 1 / 2.2) * 255), 0), 255);
    }

    int clamp(double v) {
      return std::min(std::max(0, int(v)), 255);
    }
}