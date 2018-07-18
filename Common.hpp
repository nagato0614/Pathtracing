//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_COMMON_HPP
#define PATHTRACING_COMMON_HPP

#include <random>
#include <tuple>
#include "Vector3.hpp"

namespace nagato {

    enum class SurfaceType {
        Diffuse,
        Mirror,
        Fresnel,
    };

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
    };

    int clamp(double v) {
      return std::min(std::max(0, int(v)), 255);
    }

    class Random {
    public:
        std::mt19937 engine;
        std::uniform_real_distribution<double> dist;

        Random() {};

        Random(int seed) {
          engine.seed(seed);
          dist.reset();
        }

        double next() { return dist(engine); }
    };
}
#endif //PATHTRACING_COMMON_HPP
