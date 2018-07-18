// オブジェクトローダーの読み込み関連
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_DOUBLE
#define _USE_MATH_DEFINES

#include <iostream>
#include <cstdio>
#include <omp.h>
#include <vector>
#include <optional>
#include <fstream>
#include "Vector3.hpp"
#include "Scene.hpp"
#include "TriangleMesh.hpp"
#include "Common.hpp"

using namespace nagato;

int main() {

#ifdef _DEBUG
  std::cout << "-- DEBUF MODE --" << std::endl;
#endif

#ifndef _DEBUG
  std::cout << "-- REREASE MODE --" << std::endl;
#endif

  // スレッド数の表示
#ifdef _OPENMP
  std::cout << "-- openMP --" << std::endl;
  std::cout << "The number of processors is " << omp_get_num_procs() << std::endl;
  std::cout << "OpenMP : Enabled (Max # of threads = " << omp_get_max_threads() << ")" << std::endl;
  omp_set_num_threads(omp_get_max_threads());
#else
  std::cout << "OpenMP : OFF" << std::endl;
#endif

  // Image size
  const int width = 480;
  const int height = 360;

  // Samples per pixel
  const int samples = 2;

  // Camera parameters
  const Vector3 eye(0, 5, 22);
  const Vector3 center = eye + Vector3(0, 0, -1);

  const Vector3 up(0, 1, 0);
  const double fov = 30 * M_PI / 180;
  const double aspect = double(width) / height;

  // Basis vectors for camera coordinates
  const auto wE = normalize(eye - center);
  const auto uE = normalize(cross(up, wE));
  const auto vE = cross(wE, uE);

  // シーンの読み込み
  Scene scene;
  scene.spheres.push_back(new TriangleMesh("./models/left.obj",
                                           "./models/left.mtl",
                                           Vector3(),
                                           SurfaceType::Diffuse,
                                           Vector3(.75, .25, .25)));
  scene.spheres.push_back(new TriangleMesh("./models/right_plane.obj",
                                           "./models/right_plane.mtl",
                                           Vector3(),
                                           SurfaceType::Diffuse,
                                           Vector3(.25, .25, .75)));
  scene.spheres.push_back(new TriangleMesh("./models/back_ceil_floor_plane.obj",
                                           "./models/back_ceil_floor_plane.mtl",
                                           Vector3(),
                                           SurfaceType::Diffuse,
                                           Vector3(.75, .75, .75)));
  scene.spheres.push_back(new TriangleMesh("./models/suzanne.obj",
                                           "./models/suzanne.mtl",
                                           Vector3(),
                                           SurfaceType::Diffuse,
                                           Vector3(0.8, 0.3, 0.3)));
  std::vector<Vector3> I(width * height);
  std::vector<Vector3> nom(width * height);
  std::vector<Vector3> depth_buffer(width * height);

  std::cout << "-- Image Size --" << std::endl;
  std::cout << "width : height = " << width << " : " << height << std::endl;
  std::cout << "-- RENDERING START --" << std::endl;

  for (int pass = 0; pass < samples; pass++) {
    std::cout << "\rpath : " << (pass + 1) << " / " << samples;
    fflush(stdout);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
#endif
    for (int i = 0; i < width * height; i++) {
#ifdef _OPENMP
      thread_local Random rng(42 + omp_get_thread_num() + pass);
#else
        Random rng(42 + pass + i);
#endif
      const int x = i % width;
      const int y = height - i / width;
      Ray ray;
      ray.origin = eye;
      ray.direction = [&]() {
          const double tf = std::tan(fov * .5);
          const double rpx = 2. * (x + rng.next()) / width - 1;
          const double rpy = 2. * (y + rng.next()) / height - 1;
          const Vector3 ww = normalize(
                  Vector3(aspect * tf * rpx, tf * rpy, -1));
          return uE * ww.x + vE * ww.y + wE * ww.z;
      }();

      Vector3 L(0), th(1);
      for (int depth = 0; depth < 10; depth++) {
        // Intersection
        const auto intersect = scene.intersect(
                ray, 1e-4, 1e+100);
        if (!intersect) {
          break;
        }

        if (pass == 0 && depth == 0) {
          nom[i] = (normalize(intersect->normal) + 1.0) / 2.0 * 255;
          auto d = intersect->distance;
          depth_buffer[i] = {d, d, d};
        }
        // Add contribution
        L = L + th * intersect->sphere->emittance;
        // Update next direction
        ray.origin = intersect->point;
        ray.direction = [&]() {
            if (intersect->sphere->type == SurfaceType::Diffuse) {
              // Sample direction in local coordinates
              const auto n =
                      dot(intersect->normal, -ray.direction) > 0 ? intersect->normal : -intersect
                              ->normal;
              const auto&[u, v] = tangentSpace(n);
              const auto d = [&]() {
                  const double r = sqrt(rng.next());
                  const double t = 2 * M_PI * rng.next();
                  const double x = r * cos(t);
                  const double y = r * sin(t);
                  return Vector3(x, y,
                                 std::sqrt(
                                         std::max(.0, 1 - x * x - y * y)));
              }();
              // Convert to world coordinates
              return u * d.x + v * d.y + n * d.z;
            } else if (intersect->sphere->type == SurfaceType::Mirror) {
              const auto wi = -ray.direction;
              return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
            } else if (intersect->sphere->type == SurfaceType::Fresnel) {
              const auto wi = -ray.direction;
              const auto into = dot(wi, intersect->normal) > 0;
              const auto n = into ? intersect->normal : -intersect->normal;
              const auto ior = intersect->sphere->ior;
              const auto eta = into ? 1 / ior : ior;
              const auto wt = [&]() -> std::optional<Vector3> {
                  const auto t = dot(wi, n);
                  const auto t2 = 1 - eta * eta * (1 - t * t);
                  if (t2 < 0) {
                    return {};
                  };
                  return (n * t - wi) * eta - n * sqrt(t2);
              }();
              if (!wt) {
                return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
              }
              const auto Fr = [&]() {
                  const auto cos = into ? dot(wi, intersect->normal) : dot(*wt, intersect->normal);
                  const auto r = (1 - ior) / (1 + ior);
                  return r * r + (1 - r * r) * pow(1 - cos, 5);
              }();

              return rng.next() < Fr ?
                     intersect->normal * 2 * dot(wi, intersect->normal) * intersect->normal - wi
                                     : *wt;
            }
        }();

        // Update throughput
        th = th * intersect->sphere->color;
        if (std::max(std::max(th.x, th.y), th.z) == 0) {
          break;
        }
      }
      I[i] = I[i] + L / samples;
    }
  }
  std::ofstream ofs("result.ppm");
  ofs << "P3\n" << width << " " << height << "\n255\n";
  for (const auto &i : I) {
    ofs << tonemap(i.x) << " "
        << tonemap(i.y) << " "
        << tonemap(i.z) << "\n";
  }

  // 法線マップを出力
  // [-1,1] を [0,255]に変換している
  std::ofstream output_normal("normal.ppm");
  output_normal << "P3\n" << width << " " << height << "\n255\n";
  for (const auto &i : nom) {
    output_normal << clamp(i.x) << " "
                  << clamp(i.y) << " "
                  << clamp(i.z) << "\n";
  }


  // デプスマップ用に正規化
  double max = 0;
  for (const auto &i : depth_buffer) {
    if (i.x > max) {
      max = i.x;
    }
  }

  for (auto &i : depth_buffer) {
    i = i / max;
  }

  // depthを出力
  std::ofstream output_depth("depth.ppm");
  output_depth << "P3\n" << width << " " << height << "\n255\n";
  for (const auto &i : depth_buffer) {
    output_depth << tonemap(i.x) << " "
                 << tonemap(i.y) << " "
                 << tonemap(i.z) << "\n";
  }
  std::cout << "\n-- FINISH --" << std::endl;
  return 0;
}
