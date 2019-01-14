//
// Created by 長井亨 on 2018/07/18.
//

#include <fstream>
#include "Hit.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "tiny_obj_loader.h"
#include "../object/Triangle.hpp"

namespace nagato {

    std::optional<Hit> Scene::intersect(Ray &ray, float tmin, float tmax) {
        std::optional<Hit> minh;
        for (auto &sphere : objects) {
            auto h = sphere->intersect(ray, tmin, tmax);
            if (!h) {
                continue;
            }
            minh = h;
            tmax = minh->getDistance();
        }
        return minh;
    }

    void Scene::loadObject(const std::string &objfilename,
                           const std::string &mtlfilename,
                           Material *m) {
        // オブジェクトファイルを読み込み
        std::ifstream cornellbox(objfilename);
        std::ifstream cornellob_material(mtlfilename);

        if (cornellbox.fail()) {
            std::cerr << "オブジェクトファイルを開けませんでした : "
                      << objfilename << std::endl;
            exit(-1);
        }

        if (cornellob_material.fail()) {
            std::cerr << "マテリアルファイルを開けませんでした : "
                      << mtlfilename << std::endl;
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
        tinyobj::MaterialStringStreamReader
                materialStringStreamReader(materialFileString);

        std::string err;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        tinyobj::LoadObj(&attrib,
                         &shapes,
                         &materials,
                         &err,
                         &objectFileStream,
                         &materialStringStreamReader);
        if (!err.empty()) {
            std::cerr << "error : " << err << std::endl;
        }

        for (auto &shape : shapes) {
            size_t index_offset = 0;
            auto shapeNumber = shape.mesh.num_face_vertices;

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
                size_t fnum = shape.mesh.num_face_vertices[f];

                std::vector<Vector3> p;

                // 各面の頂点に対する処理
                for (size_t v = 0; v < fnum; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    auto vertexIndex = idx.vertex_index;

                    Vector3 args(attrib.vertices[3 * vertexIndex],
                                 attrib.vertices[3 * vertexIndex + 1],
                                 attrib.vertices[3 * vertexIndex + 2]);
                    p.emplace_back(args);

                }
                index_offset += fnum;

                setObject(new Triangle(m, p));
            }
        }
    }

    void Scene::freeObject() {
        for (auto i : objects) {
            delete i;
        }
    }

    Scene::Scene() {
        objects.clear();
    }

    void Scene::setObject(Object *object) {
        if (object->getMaterial().type() == SurfaceType::Emitter) {
            lights.push_back(object);
        }

        objectCount++;
        objects.push_back(object);
    }

    int Scene::getObjectCount() const {
        return objectCount;
    }

    Spectrum Scene::directLight(Ray &ray, Hit info) {
        // 光源がない場合はエラーで終了
        if (lights.empty()) {
            exit(EXIT_CODE::EMPTY_LIGHT);
        }

        // 接続を行う光源の選択
        int lightNum = lights.size();
        auto lightPdf = 1.0f / lightNum;    // 一つの光源がサンプリングされる確率
        int selectedLight = Random::Instance().nextInt(0, lightNum - 1);
        Object *light = lights[selectedLight];

        auto sampledPoint = light->pointSampling(info);
        Ray testRay;
        testRay.origin = info.getPoint();
        testRay.direction = normalize(sampledPoint.getPoint() - info.getPoint());

        // 光源と接続点が遮られていないかテスト
        const auto intersect = this->intersect(testRay, 0.0f, 1e+100);
        if (!intersect) {
            return Spectrum(0.0f);
        } else {
            // ヒット下光源がサンプルした光源と違う場合接続を行わない
            if (intersect->getObject() != light)
                return Spectrum(0.0f);

            // 裏にあたった場合は計算を行わない.
            if (dot(-testRay.direction, intersect->getNormal()) < 0.0)
                return Spectrum(0.0f);
        }

        // 幾何項の計算
        const auto distance = (sampledPoint.getPoint() - info.getPoint()).norm();
        const auto cos_r = std::abs(dot(sampledPoint.getNormal(), -testRay.direction));
        const auto cos_i = std::abs(dot(info.getNormal(), testRay.direction));
        const auto geometry_term = (cos_r * cos_i) / distance;


        const auto &material = info.getObject()->getMaterial();
        const auto Li = light->getMaterial().emitter;
        const auto fr = material.getBSDF().f_r(-ray.direction, testRay.direction);
        const auto rho = material.color;
        const auto areaPdf = 1.0f / light->area();

        const auto Ld = (Li * geometry_term * rho) / areaPdf;

        return Ld / lightPdf;
    }
}
