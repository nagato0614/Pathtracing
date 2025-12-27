//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_COMMON_HPP
#define PATHTRACING_COMMON_HPP

#define BVH_NODE 200000

// exit code
enum EXIT_CODE
{
  EMPTY_OBJECT = 64,
  EMPTY_NODE,
  EMPTY_LIGHT,
  CANNOT_CONSTRUCT_BVH,
  OTHER,
};

// 角度の変換
#define deg_to_rad(deg) (((deg) / 360) * 2 * M_PI)
#define rad_to_deg(rad) (((rad) / 2 / M_PI) * 360)

#include <assert.h>
#include <tuple>
#include <vector>
#include "color/Spectrum.hpp"
#include "linearAlgebra/Vector3.hpp"

namespace nagato
{
class Spectrum;

std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n);

int tonemap(float v);

int clamp(float v);

float clamp(const float min, const float max, const float x);

/**
 * 線形補間を行う関数
 * @param t
 * @param a
 * @param b
 * @return
 */
float linearInterpolate(float t, float a, float b);

std::vector<int> make_rand_array_unique(size_t size, int rand_min, int rand_max, int seed);

std::string getNowTimeString();

void writePPM(std::string filename,
              std::vector<Spectrum> s,
              int width,
              int height,
              Spectrum xbar,
              Spectrum ybar,
              Spectrum zbar);

/**
 * 画像の座標系からindexに変換する
 * 左上が原点
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
inline size_t toIndex(const size_t x, const size_t y, const size_t width, const size_t height)
{
  assert(0 <= x && x < width);
  assert(0 <= y && y < height);

  return y * width + x;
}

inline std::pair<size_t, size_t> toXY(const size_t index, const size_t width, const size_t height)
{
  assert(0 <= index && index < width * height);
  size_t x = index % width;
  size_t y = index / width;
  return {x, y};
}

Vector3 sampleDirectionUniformly();

float lerp(float t, float a, float b);
} // namespace nagato
#endif // PATHTRACING_COMMON_HPP
