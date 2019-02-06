
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
#include "src/render/RenderBase.hpp"
#include "src/render/Pathtracing.hpp"
#include "src/sky/UniformSky.hpp"

using namespace nagato;

int main() {

#ifdef MY_DEBUG
    std::cout << "-- DEBUF MODE --" << std::endl;
#endif

#ifndef MY_DEBUG
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
    const int samples = 100;

    // Camera parameters
    const Vector3 eye(0, 5, 14);
    const auto center = eye + Vector3(0, 0, -1);

    const Vector3 up(0, 1, 0);
    const float fov = 55 * M_PI / 180;


    std::cout << "-- Load Scene -- " << std::endl;

    auto d65_spd = loadSPDFile("../property/cie_si_d65.csv");

    // マテリアルの読み込み
    Diffuse redMaterial(Spectrum("../property/macbeth_15_red.csv"));
    Diffuse blueMateral(Spectrum("../property/macbeth_13_blue.csv"));
    Diffuse whiteMaterial(Spectrum("../property/macbeth_18_cyan.csv"));
    Diffuse purpleMaterial(Spectrum("../property/macbeth_10_purple.csv"));
    DiffuseLight d65(d65_spd, 10);
    Mirror mirror(Spectrum(0.99));
    Glass fresnel(Spectrum(0.99), 1.5);
    UniformSky sky(Spectrum("../property/macbeth_03_blue_sky.csv"));

    // #TODO シーンファイルの読み込みモジュールの追加
    // シーンの読み込み
    BVH bvh;
    bvh.setObject(new Sphere{Vector3(-2, 2, -1), 1.1, &mirror});
    bvh.setObject(new Sphere{Vector3(2, 2, -1), 1.5, &fresnel});

//    bvh.loadObject("../models/left.obj",
//                   "../models/left.mtl", &redMaterial);
//    bvh.loadObject("../models/right.obj",
//                   "../models/right.mtl", &blueMateral);
    bvh.loadObject("../models/floor.obj",
                   "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
//    bvh.loadObject("../models/light_plane.obj",
//                   "../models/light_plane.mtl", &d65);
//    bvh.loadObject("../models/low_poly_bunny.obj",
//                   "../models/low_poly_bunny.mtl", &Fresnel);

    bvh.setSky(&sky);
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

    Pathtracing pathtracing(&bvh, &film, &pinholeCamera, samples);

    Timer timer;
    timer.start();
    pathtracing.render();
    timer.stop();
    std::cout << "\n-- Rendering Time --" << std::endl;
    std::cout << timer.getTime<>() << "[sec]" << std::endl;

    // デバッグビルド時の処理
#ifdef MY_DEBUG
    for (int i = 0; i < width * height; i++) {
        const int x = i % width;
        const int y = height - i / width;
        auto ray = pinholeCamera.makePrimaryRay(x, y);


        // Intersection
        const auto intersect = bvh.intersect(ray, 0.0f, 1e+100);

        if (!intersect) {
            break;
        }

        nom[i] = (normalize(intersect->getNormal()) + 1.0) / 2.0 * 255;
        auto d = intersect->getDistance();
        depth_buffer[i] = {d, d, d};


    }

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
#endif

    std::cout << "-- Memory release -- " << std::endl;

    bvh.freeObject();

    std::cout << "-- FINISH --" << std::endl;

    return 0;
}
