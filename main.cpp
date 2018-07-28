
#include <iostream>
#include <omp.h>
#include <cmath>
#include <vector>
#include <optional>
#include <fstream>
#include <chrono>
#include "src/Sphere.hpp"
#include "src/Scene.hpp"
#include "src/ColorRGB.hpp"
#include "src/Material.hpp"

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

    // #TODO カメラクラスの作成
    // Image size
    const int width = 480;
    const int height = 360;

    // Samples per pixel
    const int samples = 10;

    // Camera parameters
    const Vector3 eye(0, 5, 6);
    const Vector3 center = eye + Vector3(0, -0.5, -1);

    const Vector3 up(0, 1, 0);
    const float fov = 55 * M_PI / 180;
    const float aspect = float(width) / height;

    // Basis vectors for camera coordinates
    const auto wE = normalize(eye - center);
    const auto uE = normalize(cross(up, wE));
    const auto vE = cross(wE, uE);

    std::cout << "-- Load Scene -- " << std::endl;

    // マテリアルの読み込み
    Material redMaterial(SurfaceType::Diffuse, Spectrum("../property/macbeth_15_red.csv"));
    Material blueMateral(SurfaceType::Diffuse, Spectrum("../property/macbeth_13_blue.csv"));
    Material whiteMaterial(SurfaceType::Diffuse, Spectrum("../property/macbeth_19_white.csv"));
    Material d65(SurfaceType::Diffuse, Spectrum(), Spectrum("../property/cie_si_d65.csv"));
    Material mirror(SurfaceType::Mirror, Spectrum(0.99));
    Material Fresnel(SurfaceType::Fresnel, Spectrum(0.99));

    // #TODO シーンファイルの読み込みモジュールの追加
    // シーンの読み込み
    Scene scene;
    scene.objects.push_back(new Sphere{Vector3(-2, 1, 0), 1.1, &mirror});
    scene.objects.push_back(new Sphere{Vector3(2, 1, 0), 1.1, &Fresnel});
    scene.loadObject("../models/left.obj",
                     "../models/left.mtl", &redMaterial);
    scene.loadObject("../models/right.obj",
                     "../models/right.mtl", &blueMateral);
    scene.loadObject("../models/back_ceil_floor_plane.obj",
                     "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
    scene.loadObject("../models/light_plane.obj",
                     "../models/light_plane.mtl", &d65);
//  scene.objects.push_back(new TriangleMesh("../models/suzanne.obj",
//                                           "../models/suzanne.mtl",
//                                           Vector3(),
//                                           SurfaceType::Diffuse,
//                                           Vector3(0.8, 0.3, 0.3)));

    // スペクトルからXYZに変換する等色関数
    const Spectrum xbar("../property/cie_sco_2degree_xbar.csv");
    const Spectrum ybar("../property/cie_sco_2degree_ybar.csv");
    const Spectrum zbar("../property/cie_sco_2degree_zbar.csv");

    // #TODO 屈折率と反射率で異なるスペクトルクラスの実装
    // 屈折率
    Spectrum refraction("../property/SiO2.csv");

    // レンダリングした画像を保存するディレクトリを作成
    bool isOutput = false;
    auto saveDirName = "results_" + getNowTimeString();
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

    // #TODO　pathtracingクラスの作成
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
                const float tf = std::tan(fov * .5);
                const float rpx = 2. * (x + rng.next()) / width - 1;
                const float rpy = 2. * (y + rng.next()) / height - 1;
                const Vector3 ww = normalize(
                        Vector3(aspect * tf * rpx, tf * rpy, -1));
                return uE * ww.x + vE * ww.y + wE * ww.z;
            }();

            // スペクトルの最終的な寄与
            Spectrum spectrumL(0.0);

            // 各パスごとにサンプルする波長を変化させる
            Spectrum sampledSpectrum(1.0);
//            sampledSpectrum.sample(100 + i + pass);

            for (int depth = 0; depth < 10; depth++) {

                // #TODO BVHの実装
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

                if (dot(-ray.direction, intersect->normal) > 0.0) {
                    // スペクトル寄与を追加する
                    spectrumL = spectrumL + sampledSpectrum * intersect->sphere->material->emitter;
                }

                // Update next direction
                ray.origin = intersect->point;
                ray.direction = [&]() {
                    if (intersect->sphere->material->type() == SurfaceType::Diffuse) {
                        // Sample direction in local coordinates
                        const auto n =
                                dot(intersect->normal, -ray.direction) > 0 ? intersect->normal : -intersect
                                        ->normal;
                        const auto&[u, v] = tangentSpace(n);
                        const auto d = [&]() {
                            const auto r = sqrt(rng.next());
                            const auto t = 2 * M_PI * rng.next();
                            const auto x = r * cos(t);
                            const auto y = r * sin(t);
                            return Vector3((float) x, (float) y, std::sqrt(std::max(float(0.0), static_cast<const float &>(
                                    1.0 - x * x - y * y))));
                        }();
                        // Convert to world coordinates
                        return u * d.x + v * d.y + n * d.z;
                    } else if (intersect->sphere->material->type() == SurfaceType::Mirror) {
                        const auto wi = -ray.direction;
                        return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
                    } else if (intersect->sphere->material->type() == SurfaceType::Fresnel) {

                        int wavelength = 0;
//
//                        // サンプル点を１つにしてそれ以外の影響を0にする
//                        if (sampledSpectrum.samplePoints.size() > 1) {
//                            wavelength = sampledSpectrum.samplePoints[rng.next(0, SAMPLE - 1)];
//                            sampledSpectrum.leaveOnePoint(wavelength);
//                            spectrumL.leaveOnePoint(wavelength);
//                        }
//
//                        // 屈折率を取得
//                        const float ior = refraction.spectrum[wavelength];
                        const float ior = 1.5;

                        const auto wi = -ray.direction;
                        const auto into = dot(wi, intersect->normal) > 0;
                        const auto n = into ? intersect->normal : -intersect->normal;
//                        const auto ior = intersect->sphere->ior;
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
                    } else {
                        return Vector3();
                    }
                }();

                // Update throughput
                sampledSpectrum = sampledSpectrum * intersect->sphere->material->color;
                if (sampledSpectrum.findMaxSpectrum() == 0) {
                    break;
                }
            }
            // 各波長の重みを更新(サンプリング数に応じて重みをかける)
            S[i] = S[i] + (spectrumL / samples) * (RESOLUTION / sampledSpectrum.samplePoints.size());
        }

        if ((pass) % 5 == 0 && isOutput) {
            std::string outputfile = "./" + saveDirName + "/result_" + std::to_string(pass) + ".ppm";
            writePPM(outputfile, S, width, height, xbar, ybar, zbar);
        }
    }
    end = std::chrono::system_clock::now();  // 計測終了時間
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
    std::cout << "\n-- Rendering Time --" << std::endl;
    std::cout << elapsed / 1000.0 << "[sec]" << std::endl;

    std::cout << "-- Output ppm File --" << std::endl;
    writePPM("result.ppm", S, width, height, xbar, ybar, zbar);

    // #TODO 法線マップとデプスマップを出力するモジュールの実装または外部ライブラリの実装
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
    float max = 0;
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
