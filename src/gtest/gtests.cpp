//
// Created by 長井亨 on 2018/09/13.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../material/Material.hpp"
#include "../core/scene.hpp"
#include "../structure/BVH.hpp"
#include "../object/sphere.hpp"


namespace nagatTest
{
    using namespace nagato;

    /**
     * BVHと線形探索を比較しヒット判定をテスト
     */
    TEST(BVH_test, intersection)
    {

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

        BVH bvh(scene.objects);
        bvh.constructBVH();

        // Image size
        const int width = 480;
        const int height = 360;

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

        for (int i = 0; i < width * height; i++) {
            const int x = i % width;
            const int y = height - i / width;
            Ray ray;
            ray.origin = eye;
            ray.direction = [&]() {
                const float tf = std::tan(fov * .5);
                const float rpx = 2. * (x + 0.5) / width - 1;
                const float rpy = 2. * (y + 0.5) / height - 1;
                const Vector3 ww = normalize(
                        Vector3(aspect * tf * rpx, tf * rpy, -1));
                return uE * ww.x + vE * ww.y + wE * ww.z;
            }();

            const auto intersect = scene.intersect(ray, 1e-4, 1e+100);
            const auto intersectBVH = bvh.intersect(ray, 1e-4, 1e+100);

            // 線形探索とBVHの判定が正しいばいい
            if (intersect && intersectBVH) {
                ASSERT_EQ(intersect->distance, intersectBVH->distance);
                ASSERT_EQ(intersect->normal, intersectBVH->normal);
            } else if (!intersect && !intersectBVH) {
                ASSERT_EQ(intersect->distance, intersectBVH->distance);
                ASSERT_EQ(intersect->normal, intersectBVH->normal);
            } else {
                // 片方だけがヒットしている場合はテスト失敗
                FAIL();
            }
        }
        scene.freeObject();
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
