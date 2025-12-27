//
// Created by 長井亨 on 2018/09/13.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "BSDF/Lambert.hpp"
#include "color/ColorRGB.hpp"
#include "core/Common.hpp"
#include "core/Scene.hpp"
#include "film/Film.hpp"
#include "material/Material.hpp"
#include "object/Sphere.hpp"
#include "structure/BVH.hpp"

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
    BVHTest()
    {
      // マテリアルの読み込み
      Material redMaterial(SurfaceType::Diffuse, Spectrum("../property/macbeth_15_red.csv"));
      Material blueMateral(SurfaceType::Diffuse, Spectrum("../property/macbeth_13_blue.csv"));
      Material whiteMaterial(SurfaceType::Diffuse, Spectrum("../property/macbeth_19_white.csv"));
      Material d65(SurfaceType::Diffuse, Spectrum(), Spectrum("../property/cie_si_d65.csv"), 0.5);
      Material mirror(SurfaceType::Mirror, Spectrum(0.99));
      Material Fresnel(SurfaceType::Fresnel, Spectrum(0.99));

      // シーンの読み込み
      bvh.setObject(new Sphere{Vector3(-2, 1, 0), 1.1, &mirror});
      bvh.setObject(new Sphere{Vector3(2, 1, 0), 1.1, &Fresnel});
      bvh.loadObject("../models/left.obj", "../models/left.mtl", &redMaterial);
      bvh.loadObject("../models/right.obj", "../models/right.mtl", &blueMateral);
      bvh.loadObject("../models/back_ceil_floor_plane.obj",
                     "../models/back_ceil_floor_plane.mtl",
                     &whiteMaterial);
      bvh.loadObject("../models/light_plane.obj", "../models/light_plane.mtl", &d65);
      bvh.loadObject("../models/suzanne.obj", "../models/suzanne.mtl", &whiteMaterial);

      bvh.constructBVH();
    }

    ~BVHTest() { bvh.freeObject(); }

    virtual void SetUp() {}

    virtual void TearDown() {}

    BVH bvh;
};

/**
 * BVHと線形探索を比較しヒット判定をテスト
 */
TEST_F(BVHTest, intersection)
{
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

  for (int x = 0; x < width; x++)
  {
    for (int y = height; y > 0; y--)
    {
      Ray ray;
      ray.setOrigin(eye);
      const auto firstDir = [&]()
      {
        const auto tf = std::tan(fov * 0.5f);
        const auto rpx = 2.0f * (x + Random::Instance().next()) / width - 1.0f;
        const auto rpy = 2.0f * (y + Random::Instance().next()) / height - 1.0f;
        const Vector3 ww = normalize(Vector3(aspect * tf * rpx, tf * rpy, -1));
        return uE * ww.x + vE * ww.y + wE * ww.z;
      }();
      ray.setDirection(firstDir);

      const auto intersect = bvh.intersect(ray, 1e-4, 1e+100);
      const auto intersectBVH = bvh.intersect(ray, 1e-4, 1e+100);

      if (!intersect && !intersectBVH)
      {
        continue;
      }
      else if (intersect && intersectBVH)
      {
        // 線形探索とBVHの判定が正しいばいい
        ASSERT_FLOAT_EQ(intersect->getDistance(), intersectBVH->getDistance());
        ASSERT_FLOAT_EQ(intersect->getNormal().x, intersectBVH->getNormal().x)
          << "[x, y] : " << x << ", " << y;
        ASSERT_FLOAT_EQ(intersect->getNormal().y, intersectBVH->getNormal().y)
          << "[x, y] : " << x << ", " << y;
        ASSERT_FLOAT_EQ(intersect->getNormal().z, intersectBVH->getNormal().z)
          << "[x, y] : " << x << ", " << y;
        ASSERT_FLOAT_EQ(intersect->getPoint().x, intersectBVH->getPoint().x)
          << "[x, y] : " << x << ", " << y;
        ASSERT_FLOAT_EQ(intersect->getPoint().y, intersectBVH->getPoint().y)
          << "[x, y] : " << x << ", " << y;
        ASSERT_FLOAT_EQ(intersect->getPoint().z, intersectBVH->getPoint().z)
          << "[x, y] : " << x << ", " << y;
      }
    }
  }
}

/**
 * sceneとBVH内にあるオブジェクトの個数を比較
 */
TEST_F(BVHTest, existsObjectInNode)
{
  auto nodes = bvh.getNodes();
  auto nodeCount = bvh.getNodeCount();

  int existsBVHObjectCount = 0;
  for (int i = 0; i < nodeCount; i++)
  {
    if (nodes[i].object != nullptr)
    {
      existsBVHObjectCount++;
    }
  }

  int existsObjectCount = 0;
  for (auto o : bvh.objects)
  {
    if (o != nullptr)
      existsObjectCount++;
  }

  ASSERT_EQ(existsObjectCount, existsObjectCount);
}

/**
 * BVHのツリーにおいて葉にオブジェクトがあるか確認
 * 逆に中間ノードはオブジェクトを持たず子を持つか確認
 */
TEST_F(BVHTest, HasObjectAtLeaf)
{
  if (bvh.getNodeCount() == 0)
    SUCCEED();
  auto nodes = bvh.getNodes();
  for (int i = 0; i < bvh.getNodeCount(); i++)
  {
    auto *node = &nodes[i];

    // オブジェクトがない時片方だけでも子を保つ必要がある
    if ((node->object == nullptr) && (node->left == -1) && (node->right == -1))
      FAIL() << "Index : " << i;

    // オブジェクトを持つ時必ず子を持っては行けない
    else if ((node->object != nullptr) && (node->left != -1) && (node->right != -1))
      FAIL() << "Index : " << i;

    else if ((node->object != nullptr) && (node->left == -1) && (node->right != -1))
      FAIL() << "Index : " << i;

    else if ((node->object != nullptr) && (node->left != -1) && (node->right == -1))
      FAIL() << "Index : " << i;
  }
}

/**
 * BVHNodeが正しくマテリアルを保持しているかテスト
 */
TEST_F(BVHTest, ObjectHasMaterial)
{
  auto objectCount = bvh.getNodeCount();
  auto nodes = bvh.getNodes();

  for (int i = 0; i < objectCount; i++)
  {
    if (nodes[i].object != nullptr)
      ASSERT_TRUE(&nodes[i].object->getMaterial() != nullptr);
  }
}

/**
 * BSDFクラスのテストフィクスチャ
 */
class BSDFTest : public ::testing::Test
{
  protected:
    BSDFTest() {}

    ~BSDFTest() {}
};

/**
 * 半径radiusの球の一点をサンプリングする
 * 中心は原点
 * @param radius サンプリングしたい級の半径
 * @return サンプリングした座標
 */
Vector3 samplingSphere(float radius)
{
  auto &rand = Random::Instance();
  float u = rand.nextFloat(0.0, 1.0);
  float v = rand.nextFloat(0.0, 1.0);

  auto shita = deg_to_rad(180.0 * u - 90.0);
  auto phi = deg_to_rad(360.0 * v);

  auto x = radius * sin(shita) * cos(phi);
  auto y = radius * sin(shita) * sin(phi);
  auto z = cos(shita);

  return Vector3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
}

TEST_F(BSDFTest, Specular)
{
  Material material{SurfaceType::Mirror, Spectrum(1.0)};
  Sphere sphere(Vector3{0, 0, 0}, 1, &material);
}

/**
 * samplingSphereが正しく球の表面をサンプリングできているか調べる
 */
TEST(Random, SampleingSphereTest)
{
  int itr = 10000;
  for (int i = 0; i < itr; i++)
  {
    auto p = samplingSphere(1);
    auto dis = std::sqrt(p.norm());
    ASSERT_FLOAT_EQ(1.0, dis);
  }
}

TEST(Random, nextFloatTest)
{
  auto &random = Random::Instance();

  for (int i = 0; i < 10000; i++)
  {
    auto randNum = random.nextFloat(0.0, 1.0);
    ASSERT_TRUE((0 <= randNum) && (randNum <= 1.0));
  }
}

/**
 * nextfloatの取得平均が許容範囲内か調べる
 */
TEST(Random, nextFloatUniformTest)
{
  auto &random = Random::Instance();

  float sum = 0.0;
  int itr = 1000000;
  for (int i = 0; i < itr; i++)
  {
    sum += random.nextFloat(0.0, 1.0);
  }
  sum /= itr;
  ASSERT_NEAR(0.5, sum, 0.001);
}

TEST(Specturm, spectrumToRGB)
{
  const Spectrum d65("../property/cie_si_d65.csv");
  const Spectrum blue("../property/macbeth_13_blue.csv");
  const auto blue_plane = d65 * blue;
  Film film(480, 360);
  for (size_t i = 0; i < film.getPixelSize(); i++)
  {
    film[i] = blue_plane;
  }
  auto filmPixel = film.toRGB();

  // スペクトルからXYZに変換する等色関数
  const Spectrum xbar("../property/cie_sco_2degree_xbar.csv");
  const Spectrum ybar("../property/cie_sco_2degree_ybar.csv");
  const Spectrum zbar("../property/cie_sco_2degree_zbar.csv");

  ColorRGB pixelColor;
  pixelColor.spectrum2rgb(blue_plane, xbar, ybar, zbar);
  ASSERT_NEAR(pixelColor.r, filmPixel[0].r, 0.001);
  ASSERT_NEAR(pixelColor.g, filmPixel[0].g, 0.001);
  ASSERT_NEAR(pixelColor.b, filmPixel[0].b, 0.001);

  film.outputImage("test.png");
}

// BRDFのテスト
// 参照 https://zin-box.blogspot.com/2016/08/brdf.html
TEST(LambertTest, EnergyConservationTest)
{
  Lambert lambert(Spectrum(0.5));
  constexpr int N = 100000;
  constexpr auto inv_N = static_cast<float>(1 / N);
  Vector3 normal{0, 0, 1.0f};
  for (int i = 0; i < N; i++)
  {
    float energySum = 0.0;
    const Vector3 vin = sampleDirectionUniformly();
    for (int o = 0; o < N; o++)
    {
      const Vector3 vout = sampleDirectionUniformly();
      const float f = lambert.f(vin, vout);
      ASSERT_LE(0.0, f);
      const float cosTheta = dot(normal, vout);
      energySum += f * cosTheta * (2.0f * M_PI);
    }
    const float energy = energySum * inv_N;
    ASSERT_GE(1.0f, energy);
  }
}

TEST(LambertTest, HelmHoltzReciprocityTest)
{
  Lambert lambert(Spectrum(0.5));
  constexpr int N = 100000;
  constexpr float error = 1.0e-1;
  for (int i = 0; i < N; i++)
  {
    const auto vin = sampleDirectionUniformly();
    const auto vout = sampleDirectionUniformly();

    const float f1 = lambert.f(vin, vout);
    const float f2 = lambert.f(-vout, -vin);
    ASSERT_NEAR(f1, f2, error);
  }
}

TEST(LambertTest, PdfTest)
{
  Lambert lambert(Spectrum(0.5));
  constexpr int N = 100000;
  constexpr auto inv_N = 1.0 / static_cast<float>(N);
  constexpr double error = 1.0e-1;
  Hit hitpoint{0, Vector3{}, Vector3{0, 0, 1.0}, nullptr};
  for (int i = 0; i < N; i++)
  {
    auto pdfSum = 0.0;
    const auto vin = sampleDirectionUniformly();

    for (int o = 0; o < N; o++)
    {
      const auto vout = sampleDirectionUniformly();
      const auto pdf = lambert.pdf(vin, vout, hitpoint);

      pdfSum += pdf * (2.0 * M_PI);
      ASSERT_LE(pdf, 1.0) << "pdf : " << pdf;
      ASSERT_LE(0.0, pdf) << "pdf : " << pdf;
    }

    const auto pdf = pdfSum * inv_N;
    ASSERT_NEAR(1.0f, pdf, error) << "pdf : " << pdf << "\nerror : " << error;
  }
}
} // namespace nagatoTest
} // namespace nagato

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
