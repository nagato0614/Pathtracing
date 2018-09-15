//
// Created by 長井亨 on 2018/09/13.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../material/Material.hpp"
#include "../core/scene.hpp"
#include "../structure/BVH.hpp"
#include "../object/sphere.hpp"


namespace nagato
{
    namespace nagatoTest
    {

        /**
         * BVHのテストフィクスチャ
         */
        class BVHTest : public ::testing::Test
        {
         protected:
            virtual void SetUp() {
                // マテリアルの読み込み
                Material redMaterial(SurfaceType::Diffuse, Spectrum("../property/macbeth_15_red.csv"));
                Material blueMateral(SurfaceType::Diffuse, Spectrum("../property/macbeth_13_blue.csv"));
                Material whiteMaterial(SurfaceType::Diffuse, Spectrum("../property/macbeth_19_white.csv"));
                Material d65(SurfaceType::Diffuse, Spectrum(), Spectrum("../property/cie_si_d65.csv"), 0.5);
                Material mirror(SurfaceType::Mirror, Spectrum(0.99));
                Material Fresnel(SurfaceType::Fresnel, Spectrum(0.99));

                // シーンの読み込み
                scene.loadObject("../models/left.obj",
                                 "../models/left.mtl", &redMaterial);
                scene.loadObject("../models/right.obj",
                                 "../models/right.mtl", &blueMateral);
                scene.loadObject("../models/back_ceil_floor_plane.obj",
                                 "../models/back_ceil_floor_plane.mtl", &whiteMaterial);
                scene.loadObject("../models/light_plane.obj",
                                 "../models/light_plane.mtl", &d65);

                scene.loadObject("../models/teapod.obj",
                                 "../models/teapod.mtl", &whiteMaterial);

                bvh.setObject(scene.objects);
                bvh.constructBVH();

            }

            Scene scene;
            BVH bvh;

            // Image size
            int width = 480;
            int height = 360;

            // Camera parameters
            Vector3 eye{0, 5, 6};
            Vector3 center = eye + Vector3(0, -0.5, -1);

            Vector3 up{0, 1, 0};
            float fov = 55 * M_PI / 180;
            float aspect = width / height;

            // Basis vectors for camera coordinates
            Vector3 wE = normalize(eye - center);
            Vector3 uE = normalize(cross(up, wE));
            Vector3 vE = cross(wE, uE);
        };

        /**
         * BVHと線形探索を比較しヒット判定をテスト
         */
        TEST_F(BVHTest, intersection)
        {
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
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
