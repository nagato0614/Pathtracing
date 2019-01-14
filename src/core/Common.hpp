//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_COMMON_HPP
#define PATHTRACING_COMMON_HPP

#define RESOLUTION 40
#define SAMPLE 1

#define BVH_NODE 200000

// exit code
enum EXIT_CODE {
  EMPTY_OBJECT = 64,
  EMPTY_NODE ,
  EMPTY_LIGHT ,
  CANNOT_CONSTRUCT_BVH ,
  OTHER,
};

// 角度の変換
#define deg_to_rad(deg) (((deg)/360)*2*M_PI)
#define rad_to_deg(rad) (((rad)/2/M_PI)*360)

#include <tuple>
#include <vector>
#include "../linearAlgebra/Vector3.hpp"
#include "../color/Spectrum.hpp"

namespace nagato
{
    class Spectrum;

    std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n);

    int tonemap(float v);

    int clamp(float v);

    float clamp(const float min, const float max, const float x);

    std::vector<int> make_rand_array_unique(size_t size, int rand_min, int rand_max, int seed);

    std::string getNowTimeString();

    void writePPM(
            std::string filename,
            std::vector<Spectrum> s,
            int width, int height,
            Spectrum xbar, Spectrum ybar, Spectrum zbar);
}
#endif //PATHTRACING_COMMON_HPP
