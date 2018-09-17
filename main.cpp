
#include <iostream>
#include <omp.h>
#include <cmath>
#include <vector>
#include <optional>
#include <fstream>
#include <chrono>
#include "src/linearAlgebra/Vector3.hpp"
#include "src/material/Material.hpp"
#include "src/core/Scene.hpp"
#include "src/object/Sphere.hpp"
#include "src/structure/BVH.hpp"
#include "src/BSDF/Specular.hpp"

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
    const int samples = 100;

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
    Material d65(SurfaceType::Diffuse, Spectrum(), Spectrum("../property/cie_si_d65.csv"), 0.5);
    Material mirror(SurfaceType::Mirror, Spectrum(0.99));
    Material Fresnel(SurfaceType::Fresnel, Spectrum(0.99));

    // #TODO シーンファイルの読み込みモジュールの追加
    // シーンの読み込み
    Scene scene;
    scene.setObject(new Sphere{Vector3(-2, 1, 0), 1.1, &mirror});
    scene.setObject(new Sphere{Vector3(2, 1, 0), 1.1, &Fresnel});
    scene.loadObject("../models/left.obj",
                     "../models/left.mtl", &redMaterial);
    scene.loadObject("../models/right.obj",
                     "../models/right.mtl", &blueMateral);
    scene.loadObject("../models/back_ceil_floor_plane.obj",
                     "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
    scene.loadObject("../models/light_plane.obj",
                     "../models/light_plane.mtl", &d65);
//    scene.loadObject("../models/suzanne.obj",
//                     "../models/suzanne.mtl", &whiteMaterial);

    std::cout << "-- Construct BVH --" << std::endl;
    BVH bvh(scene.objects);
    bvh.constructBVH();

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

    std::cout << "-- Out Put Image Size --" << std::endl;
    std::cout << "width : height = " << width << " : " << height << std::endl;
    std::cout << "-- Number of Object --" << std::endl;
    std::cout << "objects : " << scene.objects.size() << std::endl;
    std::cout << "nodes   : " << bvh.getNodeCount() << std::endl;
    std::cout << "BVH_memory : " << bvh.getMemorySize() << std::endl;
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
            const int x = i % width;
            const int y = height - i / width;
            Ray ray;
            ray.origin = eye;
            ray.direction = [&]() {
                const float tf = std::tan(fov * .5);
                const float rpx = 2. * (x + Random::Instance().next()) / width - 1;
                const float rpy = 2. * (y + Random::Instance().next()) / height - 1;
                const Vector3 ww = normalize(
                        Vector3(aspect * tf * rpx, tf * rpy, -1));
                return uE * ww.x + vE * ww.y + wE * ww.z;
            }();

            // スペクトルの最終的な寄与
            Spectrum L(0.0);

            // 各パスにおけるウェイト
            Spectrum weight(1.0);

            // 波長ごとの追跡を行う場合における追跡している波長
            int wavelength = -1;

            bool isSlected = false;

            for (int depth = 0; depth < 10; depth++) {

                // Intersection
                const auto intersect = bvh.intersect(ray, 1e-4, 1e+100);

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
                    L = L + weight * intersect->sphere->material->emitter;
                }

                // Update next direction
                ray.origin = intersect->point;
                BSDF *bsdf = intersect->sphere->material->getBSDF();
                auto color = bsdf->makeNewDirection(&wavelength, &ray.direction, ray, intersect.value());

                // Update throughput
                weight = weight * color;
                if (weight.findMaxSpectrum() == 0) {
                    break;
                }
            }
            // 各波長の重みを更新(サンプリング数に応じて重みをかける)
            S[i] = S[i] + (L / samples);
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

    std::cout << "-- Memory release -- " << std::endl;

    scene.freeObject();

    std::cout << "-- FINISH --" << std::endl;

    return 0;
}
