
#include <iostream>
#include <omp.h>
#include <cmath>
#include <vector>
#include <optional>
#include <fstream>
#include <chrono>
#include "linearAlgebra/Vector3.hpp"
#include "material/Material.hpp"
#include "core/Scene.hpp"
#include "object/Sphere.hpp"
#include "structure/BVH.hpp"
#include "BSDF/Specular.hpp"
#include "film/Film.hpp"
#include "material/Diffuse.hpp"
#include "material/DiffuseLight.hpp"
#include "material/Glass.hpp"
#include "material/Mirror.hpp"
#include "camera/PinholeCamera.hpp"
#include "core/Timer.hpp"
#include "render/RenderBase.hpp"
#include "render/Pathtracing.hpp"
#include "sky/UniformSky.hpp"
#include "sky/SimpleSky.hpp"
#include "sky/ImageBasedLighting.hpp"

#include "core/ThreadPool.hpp"

using namespace nagato;

int main() {
#ifdef _OPENMP
    std::cout << "-- openMP --" << std::endl;
#else
    std::cout << "-- ThreadPool --" << std::endl;
#endif

#ifdef MY_DEBUG
    std::cout << "-- DEBUF MODE --" << std::endl;
#endif

#ifndef MY_DEBUG
    std::cout << "-- REREASE MODE --" << std::endl;
#endif

    // スレッド数の表示
#ifdef _OPENMP
    std::cout << "The number of processors is " << omp_get_num_procs() << std::endl;
    std::cout << "OpenMP : Enabled (Max # of threads = " << omp_get_max_threads() << ")" << std::endl;
    omp_set_num_threads(omp_get_max_threads());
#else
    std::cout << "OpenMP : OFF (Using ThreadPool)" << std::endl;
#endif

    Spectrum::initSpectrum();

    // Image size
    const int width = 1000;
    const int height = 1000;

    // Samples per pixel
    const int samples = 100;

    // Camera parameters
    const Vector3 eye(0, 5, 14);
    const auto center = eye + Vector3(0, 0, -1);

    const Vector3 up(0, 1, 0);
    const float fov = 55 * M_PI / 180;


    std::cout << "-- Load Scene -- " << std::endl;

    auto d65_spd = loadSPDFile("../property/cie_si_d65.csv");

    Spectrum skyColor = Spectrum::rgb2Spectrum({0.5, 0.7, 1});


    // マテリアルの読み込み
    Diffuse redMaterial(Spectrum("../property/macbeth_15_red.csv"));
    Diffuse blueMateral(Spectrum("../property/macbeth_13_blue.csv"));
    Diffuse whiteMaterial(Spectrum("../property/macbeth_22_neutral_5.csv"));
    Diffuse purpleMaterial(Spectrum("../property/macbeth_14_green.csv"));
    DiffuseLight d65(d65_spd, 10);
    Mirror mirror(Spectrum(0.99));
    Glass fresnel(Spectrum(0.99), 1.5);
    ImageBasedLighting sky("../texture/playa.exr");


    BVH bvh;

    /**
     * シーンの読み込み
     * レンダリングするシーンのみコメントアウトする
     * #TODO シーンファイルの読み込みモジュールの追加
     */

    // IBL playa うさぎ  ==================================================
//    bvh.loadObject("../models/floor.obj",
//                   "../models/floor.mtl", &whiteMaterial);
//    bvh.loadObject("../models/low_poly_bunny.obj",
//                   "../models/low_poly_bunny.mtl", &fresnel);
//    bvh.setSky(&sky);

    // IBL playa 球  ==================================================
//    bvh.setObject(new Sphere{Vector3(-2, 2, -1), 1.1, &purpleMaterial});
//    bvh.setObject(new Sphere{Vector3(2, 2, -1), 1.5, &fresnel});
//    bvh.loadObject("../models/floor.obj",
//                   "../models/floor.mtl", &whiteMaterial);
//    bvh.setSky(&sky);

    // コーネルボックス　うさぎ　==================================================

    bvh.loadObject("../models/left.obj",
                   "../models/left.mtl", &redMaterial);
    bvh.loadObject("../models/right.obj",
                   "../models/right.mtl", &blueMateral);
    bvh.loadObject("../models/back_ceil_floor_plane.obj",
                   "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
    bvh.loadObject("../models/light_plane.obj",
                   "../models/light_plane.mtl", &d65);
    bvh.loadObject("../models/low_poly_bunny.obj",
                   "../models/low_poly_bunny.mtl", &fresnel);

    // コーネルボックス　球   ==================================================

    // bvh.setObject(new Sphere{Vector3(-2, 2, -1), 1.1, &mirror});
    // bvh.setObject(new Sphere{Vector3(2, 2, -1), 1.5, &fresnel});
    //
    // bvh.loadObject("../models/left.obj",
    //                "../models/left.mtl", &redMaterial);
    // bvh.loadObject("../models/right.obj",
    //                "../models/right.mtl", &blueMateral);
    // bvh.loadObject("../models/back_ceil_floor_plane.obj",
    //                "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
    // bvh.loadObject("../models/light_plane.obj",
    //                "../models/light_plane.mtl", &d65);

    // ======================================================================

    std::cout << "-- Construct BVH --" << std::endl;
    bvh.constructBVH();

    // #TODO 屈折率と反射率で異なるスペクトルを扱えるクラスの実装
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
