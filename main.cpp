
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
#include "src/film/Film.hpp"
#include "src/material/Diffuse.hpp"
#include "src/material/DiffuseLight.hpp"
#include "src/material/Glass.hpp"
#include "src/material/Mirror.hpp"
#include "src/camera/PinholeCamera.hpp"
#include "src/core/Timer.hpp"

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
    const int width = 400;
    const int height = 400;

    // Samples per pixel
    const int samples = 50;

    // Camera parameters
    const Vector3 eye(0, 5, 14);
    const auto center = eye + Vector3(0, 0, -1);

    const Vector3 up(0, 1, 0);
    const float fov = 55 * M_PI / 180;


    std::cout << "-- Load Scene -- " << std::endl;

    // マテリアルの読み込み
    Diffuse redMaterial(Spectrum("../property/macbeth_15_red.csv"));
    Diffuse blueMateral(Spectrum("../property/macbeth_13_blue.csv"));
    Diffuse whiteMaterial(Spectrum("../property/macbeth_19_white.csv"));
    Diffuse purpleMaterial(Spectrum("../property/macbeth_10_purple.csv"));
    DiffuseLight d65(Spectrum("../property/cie_si_d65.csv"), 10);
    Mirror mirror(Spectrum(0.99));
    Glass fresnel(Spectrum(0.99), 1.5);

    // #TODO シーンファイルの読み込みモジュールの追加
    // シーンの読み込み
    BVH bvh;
    bvh.setObject(new Sphere{Vector3(-2, 2, -1), 1.1, &mirror});
    bvh.setObject(new Sphere{Vector3(2, 2, -1), 1.5, &fresnel});

    bvh.loadObject("../models/left.obj",
                   "../models/left.mtl", &redMaterial);
    bvh.loadObject("../models/right.obj",
                   "../models/right.mtl", &blueMateral);
    bvh.loadObject("../models/back_ceil_floor_plane.obj",
                   "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
    bvh.loadObject("../models/light_plane.obj",
                   "../models/light_plane.mtl", &d65);
//    bvh.loadObject("../models/low_poly_bunny.obj",
//                   "../models/low_poly_bunny.mtl", &Fresnel);

    std::cout << "-- Construct BVH --" << std::endl;
    bvh.constructBVH();

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

    // ピンホールカメラ
    PinholeCamera pinholeCamera{eye, up, fov, width, height};

    // 波長データを保存
    Film film(width, height);

    std::vector<Vector3> nom(width * height);
    std::vector<Vector3> depth_buffer(width * height);

    std::cout << "-- Out Put Image Size --" << std::endl;
    std::cout << "width : height = " << width << " : " << height << std::endl;
    std::cout << "-- Number of Object --" << std::endl;
    std::cout << "objects : " << bvh.objects.size() << std::endl;
    std::cout << "nodes   : " << bvh.getNodeCount() << std::endl;
    std::cout << "BVH_memory : " << bvh.getMemorySize() << std::endl;
    std::cout << "-- RENDERING START --" << std::endl;

    Timer timer;
    timer.start();
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
            auto ray = pinholeCamera.makePrimaryRay(x, y);

            // スペクトルの最終的な寄与
            Spectrum L(0.0);

            // 各パスにおけるウェイト
            Spectrum weight(1.0);

            // 波長ごとの追跡を行う場合における追跡している波長
            int wavelength = -1;

            bool isSlected = false;

            for (int depth = 0; depth < 5; depth++) {

                // Intersection
                const auto intersect = bvh.intersect(ray, 0.0f, 1e+100);

                if (!intersect) {
                    break;
                }

                if (pass == 0 && depth == 0) {
                    nom[i] = (normalize(intersect->getNormal()) + 1.0) / 2.0 * 255;
                    auto d = intersect->getDistance();
                    depth_buffer[i] = {d, d, d};
                }

                if (dot(-ray.getDirection(), intersect->getNormal()) > 0.0) {
                    // スペクトル寄与を追加する
                    L = L + weight * intersect->getObject().getMaterial().getEmitter();
                }

                // next event estimation
                auto type = intersect->getObject().getMaterial().type();
                if (type != SurfaceType::Emitter
                    && type == SurfaceType::Diffuse) {
                    L = L + weight * bvh.directLight(ray, intersect.value());
                }

                // Update next direction
                ray.setOrigin(intersect->getPoint());
                Vector3 dir;
                float pdf = 1.0;
                auto &bsdf = intersect->getObject().getMaterial().getBSDF();
                auto color = bsdf.makeNewDirection(&wavelength,
                                                   &dir,
                                                   ray,
                                                   intersect.value(),
                                                   &pdf);

                // Update throughput
                weight = weight *
                         ((color * std::abs(dot(-ray.getDirection(), dir))) / pdf);
                if (weight.findMaxSpectrum() == 0) {
                    break;
                }

                ray.setDirection(dir);


                // ロシアンルーレットで追跡を終了する
                auto maxWeight = weight.findMaxSpectrum();
                if (Random::Instance().next() < 1.0 - maxWeight && depth > 3) {
                    break;
                } else {
                    weight = weight / maxWeight;
                }
            }
            // 各波長の重みを更新(サンプリング数に応じて重みをかける)
            film[i] = film[i] + (L / samples);
        }

        if ((pass) % 5 == 0 && isOutput) {
            std::string outputfile =
                    "./" + saveDirName + "/result_" + std::to_string(pass) + ".ppm";
            film.outputImage(outputfile);
        }
    }
    timer.stop();
    std::cout << "\n-- Rendering Time --" << std::endl;
    std::cout << timer.getTime<>() << "[sec]" << std::endl;

    std::cout << "-- Output ppm File --" << std::endl;
    film.outputImage("output2.ppm");

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

    bvh.freeObject();

    std::cout << "-- FINISH --" << std::endl;

    return 0;
}
