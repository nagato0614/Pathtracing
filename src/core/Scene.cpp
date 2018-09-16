//
// Created by 長井亨 on 2018/07/18.
//

#include <fstream>
#include "Hit.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "tiny_obj_loader.h"
#include "../object/Triangle.hpp"


namespace nagato
{

    std::optional<Hit> Scene::intersect(Ray &ray, float tmin, float tmax)
    {
        std::optional<Hit> minh;
        for (auto &sphere : objects) {
            auto h = sphere->intersect(ray, tmin, tmax);
            if (!h) {
                continue;
            }
            minh = h;
            tmax = minh->distance;
        }
        return minh;
    }

    void Scene::loadObject(const std::string &objfilename, const std::string &mtlfilename, Material *m)
    {
        // オブジェクトファイルを読み込み
        std::ifstream cornellbox(objfilename);
        std::ifstream cornellob_material(mtlfilename);

        if (cornellbox.fail()) {
            std::cerr << "オブジェクトファイルを開けませんでした : "
                      << objfilename << std::endl;
            exit(-1);
        } else {
            std::cout << "load object file \t: "
                      << objfilename << std::endl;
        }

        if (cornellob_material.fail()) {
            std::cerr << "マテリアルファイルを開けませんでした : "
                      << mtlfilename << std::endl;
            exit(-1);
        } else {
            std::cout << "load material file \t: "
                      << mtlfilename << std::endl;
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

                objects.push_back(new Triangle(m, p));
            }
        }
    }

    void Scene::freeObject()
    {
        for (auto i : objects) {
            delete i;
        }
    }

    Scene::Scene()
    {
        objects.clear();
    }

    void Scene::setObject(Object *object)
    {
        objectCount++;
        objects.push_back(object);
    }
}
