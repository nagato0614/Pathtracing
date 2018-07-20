// オブジェクトローダーの読み込み関連
#define TINYOBJLOADER_USE_DOUBLE

#include <iostream>
#include <omp.h>
#include <cmath>
#include <vector>
#include <optional>
#include <fstream>
#include <chrono>
#include "Scene.hpp"
#include "TriangleMesh.hpp"
#include "Sphere.hpp"
#include "Random.hpp"
#include "Spectrum.hpp"

using namespace nagato;

int main()
{

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
    const int samples = 10;

    // Camera parameters
    const Vector3 eye(0, 5, 10);
    const Vector3 center = eye + Vector3(0, 0, -1);

    const Vector3 up(0, 1, 0);
    const double fov = 60 * M_PI / 180;
    const double aspect = double(width) / height;

    // Basis vectors for camera coordinates
    const auto wE = normalize(eye - center);
    const auto uE = normalize(cross(up, wE));
    const auto vE = cross(wE, uE);

    std::cout << "-- Load Scene -- " << std::endl;
    // シーンの読み込み
    Scene scene;
    scene.spheres.push_back(new Sphere{Vector3(-2, 1, 0), 1.1, SurfaceType::Mirror, Spectrum(0.99)});
//    scene.spheres.push_back(new Sphere{Vector3(2, 1, 0), 1.1, SurfaceType::Fresnel, Spectrum(0.99)});
    scene.spheres.push_back(new TriangleMesh("../models/left.obj",
                                             "../models/left.mtl",
                                             Vector3(),
                                             SurfaceType::Diffuse,
                                             Spectrum("../property/macbeth_15_red.csv")));
    scene.spheres.push_back(new TriangleMesh("../models/right.obj",
                                             "../models/right.mtl",
                                             Vector3(),
                                             SurfaceType::Diffuse,
                                             Spectrum("../property/macbeth_13_blue.csv"
                                             )));
    scene.spheres.push_back(new TriangleMesh("../models/back_ceil_floor_plane.obj",
                                             "../models/back_ceil_floor_plane.mtl",
                                             Vector3(),
                                             SurfaceType::Diffuse,
                                             Spectrum("../property/macbeth_19_white.csv")));
    scene.spheres.push_back(new TriangleMesh("../models/light_plane.obj",
                                             "../models/light_plane.mtl",
                                             Vector3(),
                                             SurfaceType::Diffuse,
                                             Spectrum(),
                                             Spectrum("../property/cie_si_d65.csv")));
//  scene.spheres.push_back(new TriangleMesh("../models/suzanne.obj",
//                                           "../models/suzanne.mtl",
//                                           Vector3(),
//                                           SurfaceType::Diffuse,
//                                           Vector3(0.8, 0.3, 0.3)));

    // スペクトルからXYZに変換する等色関数
    const Spectrum red("../property/cie_1931_red.csv");
    const Spectrum blue("../property/cie_1931_blue.csv");
    const Spectrum green("../property/cie_1931_green.csv");

    // レンダリングした画像を保存するディレクトリを作成
    bool isOutput = false;
    auto saveDirName = getNowTimeString() + "_results";
    auto command = "mkdir -p " + saveDirName;
    if (isOutput) {
        system(command.c_str());
    }

    // 波長データを保存
    std::vector<Spectrum> S(width * height);

    std::vector<Vector3> nom(width * height);
    std::vector<Vector3> depth_buffer(width * height);

    std::cout << "-- Image Size --" << std::endl;
    std::cout << "width : height = " << width << " : " << height << std::endl;
    std::cout << "-- RENDERING START --" << std::endl;

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now(); // 計測開始時間
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

            // スペクトルの最終的な寄与
            Spectrum spectrumL(0.0);

            // 各パスごとにサンプルする波長を変化させる
            Spectrum sampledSpectrum(1.0);
//            sampledSpectrum.sample();

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
//                L = L + th * intersect->sphere->emittance;

                // スペクトル寄与を追加する
                spectrumL = spectrumL + sampledSpectrum * intersect->sphere->emittance;

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
                            return Vector3(x, y, std::sqrt(std::max(.0, 1 - x * x - y * y)));
                        }();
                        // Convert to world coordinates
                        return u * d.x + v * d.y + n * d.z;
                    } else if (intersect->sphere->type == SurfaceType::Mirror) {
                        const auto wi = -ray.direction;
                        return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
                    } else if (intersect->sphere->type == SurfaceType::Fresnel) {
                        // #TODO : 波長を考慮した屈折の実装
//                        const auto wi = -ray.direction;
//                        const auto into = dot(wi, intersect->normal) > 0;
//                        const auto n = into ? intersect->normal : -intersect->normal;
//                        const auto ior = intersect->sphere->ior;
//                        const auto eta = into ? 1 / ior : ior;
//                        const auto wt = [&]() -> std::optional<Vector3> {
//                            const auto t = dot(wi, n);
//                            const auto t2 = 1 - eta * eta * (1 - t * t);
//                            if (t2 < 0) {
//                                return {};
//                            };
//                            return (n * t - wi) * eta - n * sqrt(t2);
//                        }();
//                        if (!wt) {
//                            return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
//                        }
//                        const auto Fr = [&]() {
//                            const auto cos = into ? dot(wi, intersect->normal) : dot(*wt, intersect->normal);
//                            const auto r = (1 - ior) / (1 + ior);
//                            return r * r + (1 - r * r) * pow(1 - cos, 5);
//                        }();
//
//                        return rng.next() < Fr ?
//                               intersect->normal * 2 * dot(wi, intersect->normal) * intersect->normal - wi
//                                               : *wt;
                    } else {
                        return Vector3();
                    }
                }();

                // Update throughput
                sampledSpectrum = sampledSpectrum * intersect->sphere->color;
                if (sampledSpectrum.findMaxSpectrum() == 0) {
                    break;
                }
            }
            // 各波長の重みを更新
            S[i] = S[i] + spectrumL / samples;
        }

        if ((pass + 1) % 5 == 0 && isOutput) {
            std::string outputfile = "./" + saveDirName + "/result_" + std::to_string(pass) + ".ppm";
            std::ofstream ofs(outputfile);
            ofs << "P3\n" << width << " " << height << "\n255\n";
            for (const auto &i : S) {
                ColorRGB pixelColor;
                pixelColor.spectrum2rgb(i, red, green, blue);
                ofs << tonemap(pixelColor.r) << " "
                    << tonemap(pixelColor.g) << " "
                    << tonemap(pixelColor.b) << "\n";
            }
        }
    }
    end = std::chrono::system_clock::now();  // 計測終了時間
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
    std::cout << "\n-- Rendering Time --" << std::endl;
    std::cout << elapsed / 1000.0 << "[sec]" << std::endl;

    std::cout << "-- Output ppm File --" << std::endl;
    std::ofstream ofs("result.ppm");
    ofs << "P3\n" << width << " " << height << "\n255\n";
    for (const auto &i : S) {
        ColorRGB pixelColor;
        pixelColor.spectrum2rgb(i, red, green, blue);
        ofs << tonemap(pixelColor.r) << " "
            << tonemap(pixelColor.g) << " "
            << tonemap(pixelColor.b) << "\n";
        std::cout << (pixelColor.r) << " "
                  << (pixelColor.g) << " "
                  << (pixelColor.b) << "\n";
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

    std::cout << "-- FINISH --" << std::endl;

    return 0;
}
