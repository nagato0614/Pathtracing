//
// Created by 長井亨 on 2018/07/18.
//

#include "core/Scene.hpp"
#include <fstream>
#include <filesystem>
#include <limits>
#include <sstream>
#include <unordered_map>
#include "color/ColorRGB.hpp"
#include "object/Triangle.hpp"
#include "core/Hit.hpp"
#include "core/Ray.hpp"
#include "core/tiny_obj_loader.h"
#include "material/Diffuse.hpp"

namespace nagato
{
namespace
{
constexpr float kShadowRayEpsilonScene = 1e-4f;
}

std::optional<Hit> Scene::intersect(Ray &ray, float tmin, float tmax)
{
  std::optional<Hit> minh;
  for (auto &obj : objects)
  {
    auto h = obj->intersect(ray, tmin, tmax);
    if (!h)
    {
      continue;
    }
    minh = h;
    tmax = minh->getDistance();
  }
  return minh;
}

void Scene::loadObject(
  const std::string &objfilename, const std::string &mtlfilename, Material *m, float scale)
{
  // オブジェクトファイルを読み込み
  std::ifstream cornellbox(objfilename);
  std::ifstream cornellob_material(mtlfilename);

  if (cornellbox.fail())
  {
    std::cerr << "オブジェクトファイルを開けませんでした : " << objfilename << std::endl;
    exit(-1);
  }

  if (cornellob_material.fail())
  {
    std::cerr << "マテリアルファイルを開けませんでした : " << mtlfilename << std::endl;
    exit(-1);
  }

  // オブジェクトファイルを文字列に変換
  std::stringstream objectFileStream;
  objectFileStream << cornellbox.rdbuf();
  cornellbox.close();

  std::stringstream strstream;
  strstream.str("");
  strstream << cornellob_material.rdbuf();
  cornellob_material.close();
  std::string materialFileString(strstream.str());

  // マテリアルローダを初期化
  tinyobj::MaterialStringStreamReader materialStringStreamReader(materialFileString);

  std::string err;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  tinyobj::LoadObj(
    &attrib, &shapes, &materials, &err, &objectFileStream, &materialStringStreamReader);
  if (!err.empty())
  {
    std::cerr << "error : " << err << std::endl;
  }

  const bool needsAutoMaterial = (m == nullptr);
  std::unordered_map<int, Material *> materialTable;
  Material *autoFallbackMaterial = nullptr;
  if (needsAutoMaterial && !materials.empty())
  {
    const std::filesystem::path materialDir = std::filesystem::path(mtlfilename).parent_path();
    materialTable.reserve(materials.size());
    for (size_t i = 0; i < materials.size(); ++i)
    {
      const auto &mat = materials[i];
      ColorRGB kd(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
      auto spectrum = Spectrum::rgb2Spectrum(kd);
      auto newMaterial = std::make_unique<Diffuse>(spectrum);

      if (!mat.diffuse_texname.empty())
      {
        std::filesystem::path texturePath = materialDir / mat.diffuse_texname;
        newMaterial->setDiffuseTexture(texturePath.lexically_normal().string());
      }

      materialTable.emplace(static_cast<int>(i), newMaterial.get());
      ownedMaterials.push_back(std::move(newMaterial));
    }

    if (!materialTable.empty())
    {
      autoFallbackMaterial = materialTable.begin()->second;
    }
  }

  if (needsAutoMaterial && autoFallbackMaterial == nullptr)
  {
    auto fallback = std::make_unique<Diffuse>(Spectrum(0.8f));
    autoFallbackMaterial = fallback.get();
    ownedMaterials.push_back(std::move(fallback));
  }

  for (auto &shape : shapes)
  {
    size_t index_offset = 0;
    auto shapeNumber = shape.mesh.num_face_vertices;

    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
    {
      size_t fnum = shape.mesh.num_face_vertices[f];

      std::vector<Vector3> p;
      std::vector<Vector2> uv;
      bool faceHasTexcoord = true;

      Material *materialForFace = needsAutoMaterial ? autoFallbackMaterial : m;
      if (needsAutoMaterial && f < shape.mesh.material_ids.size())
      {
        const int materialId = shape.mesh.material_ids[f];
        if (materialId >= 0)
        {
          auto it = materialTable.find(materialId);
          if (it != materialTable.end())
          {
            materialForFace = it->second;
          }
        }
      }

      // 各面の頂点に対する処理
      for (size_t v = 0; v < fnum; v++)
      {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
        auto vertexIndex = idx.vertex_index;

        Vector3 args(attrib.vertices[3 * vertexIndex],
                     attrib.vertices[3 * vertexIndex + 1],
                     attrib.vertices[3 * vertexIndex + 2]);
        args = args * scale;
        p.emplace_back(args);

        Vector2 texcoord(0.0f);
        if (idx.texcoord_index >= 0 && !attrib.texcoords.empty())
        {
          const size_t texIndex = static_cast<size_t>(idx.texcoord_index) * 2;
          if (texIndex + 1 < attrib.texcoords.size())
          {
            texcoord.x = attrib.texcoords[texIndex];
            texcoord.y = attrib.texcoords[texIndex + 1];
          }
          else
          {
            faceHasTexcoord = false;
          }
        }
        else
        {
          faceHasTexcoord = false;
        }
        uv.emplace_back(texcoord);
      }
      index_offset += fnum;

      setObject(new Triangle(materialForFace, p, uv, faceHasTexcoord));
    }
  }
}

void Scene::freeObject()
{
  for (auto i : objects)
  {
    delete i;
  }
  objects.clear();
  lights.clear();
  sky = nullptr;
  objectCount = 0;
  ownedMaterials.clear();
}

Scene::Scene() { objects.clear(); }

void Scene::setObject(Object *object)
{
  if (object->getMaterial().type() == SurfaceType::Emitter)
  {
    lights.push_back(object);
  }

  objectCount++;
  objects.push_back(object);
}

int Scene::getObjectCount() const { return objectCount; }

const std::vector<Object *> &Scene::getLights() const { return lights; }

Spectrum Scene::directLight(Ray &ray, Hit info)
{
  const auto areaLightCount = lights.size();
  const bool sampleSky = hasSky();

  if (areaLightCount == 0 && !sampleSky)
  {
    return Spectrum(0);
  }

  const int totalSources = static_cast<int>(areaLightCount) + (sampleSky ? 1 : 0);
  const float selectionPdf = 1.0f / static_cast<float>(totalSources);
  const int selectedIndex = Random::Instance().nextInt(0, totalSources - 1);

  const auto &material = info.getObject().getMaterial();

  if (selectedIndex < static_cast<int>(areaLightCount))
  {
    Object *light = lights[selectedIndex];

    auto sampledPoint = light->pointSampling(info);
    Ray testRay;
    testRay.setOrigin(info.getPoint());
    testRay.setDirection(normalize(sampledPoint.getPoint() - info.getPoint()));

    const auto intersect = this->intersect(testRay, 0.0f, 1e+100);
    if (!intersect)
    {
      return Spectrum(0.0f);
    }
    else
    {
      if (&intersect->getObject() != light)
        return Spectrum(0.0f);

      if (dot(-testRay.getDirection(), intersect->getNormal()) < 0.0)
        return Spectrum(0.0f);
    }

    const auto distance = (sampledPoint.getPoint() - info.getPoint()).norm();
    const auto cos_r = std::abs(dot(sampledPoint.getNormal(), -testRay.getDirection()));
    const auto cos_i = std::abs(dot(info.getNormal(), testRay.getDirection()));
    const auto geometry_term = (cos_r * cos_i) / distance;

    const auto Li = light->getMaterial().getEmitter();
    const auto rho = material.evaluateColor(&info);
    const auto areaPdf = 1.0f / light->area();

    const auto Ld = (Li * geometry_term * rho) / areaPdf;

    return Ld / selectionPdf;
  }

  if (!sampleSky)
  {
    return Spectrum(0.0f);
  }

  auto skySample = sky->sample(info.getPoint());
  if (skySample.pdf <= 0.0f)
  {
    return Spectrum(0.0f);
  }

  Ray shadowRay = skySample.ray;
  shadowRay.setOrigin(shadowRay.getOrigin() + shadowRay.getDirection() * kShadowRayEpsilonScene);
  const auto occlusion = this->intersect(shadowRay, 0.0f, std::numeric_limits<float>::max());
  if (occlusion)
  {
    return Spectrum(0.0f);
  }

  const float cos_i = std::max(0.0f, dot(info.getNormal(), shadowRay.getDirection()));
  if (cos_i <= 0.0f)
  {
    return Spectrum(0.0f);
  }

  const auto rho = material.evaluateColor(&info);
  return (skySample.radiance * rho * cos_i) / (skySample.pdf * selectionPdf);
}

void Scene::setSky(Sky *sky) { this->sky = sky; }

const Sky &Scene::getSky() { return *sky; }

bool Scene::hasSky() { return sky != nullptr; }
} // namespace nagato
