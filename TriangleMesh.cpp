//
// Created by 長井亨 on 2018/07/18.
//
#include "TriangleMesh.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cfloat>
#include <cmath>

namespace nagato {

    TriangleMesh::TriangleMesh(const std::string &objfilename, const std::string &mtlfilename, Vector3 p, SurfaceType t,
                               Vector3 color, Vector3 em)
            : Object(p, t, color, em) {
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
      tinyobj::MaterialStringStreamReader materialStringStreamReader(materialFileString);

      std::string err;
      tinyobj::LoadObj(&this->attrib,
                       &this->shapes,
                       &this->materials,
                       &err,
                       &objectFileStream,
                       &materialStringStreamReader);
      if (!err.empty()) {
        std::cerr << "error : " << err << std::endl;
      }
    }

    std::optional<Hit> TriangleMesh::intersect(Ray &ray, double tmin, double tmax) {

      std::optional<Hit> min = {};
      auto mindis = DBL_MAX;
      // すべてのポリゴンに対して辺り判定を行う(線形探索)
      // その中で最も近いものをレイの交点とする
      for (auto &shape : shapes) {
        size_t index_offset = 0;
        auto shapeNumber = shape.mesh.num_face_vertices;
        std::vector<Vector3> points;
        std::vector<Vector3> normals;


        // .objファイルに含まれている各オブジェクトに対しての処理
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
          size_t fnum = shape.mesh.num_face_vertices[f];

          points.clear();
          normals.clear();

          // 各面の頂点に対する処理
          for (size_t v = 0; v < fnum; v++) {

            // 各面の頂点情報と法線情報の取得
            tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
            auto vertexIndex = idx.vertex_index;
            auto normalIndex = idx.normal_index;
            auto vertex =
                    Vector3(attrib.vertices[3 * vertexIndex],
                            attrib.vertices[3 * vertexIndex + 1],
                            attrib.vertices[3 * vertexIndex + 2]);
            points.push_back(vertex);

            // 法線情報がない場合はスキップ
            if (normalIndex >= 0) {
              auto normal =
                      Vector3(attrib.normals[normalIndex],
                              attrib.normals[normalIndex + 1],
                              attrib.normals[normalIndex + 2]);
              normals.push_back(normal);
            }
          }

          const auto normal = normalize(cross(points[1] - points[0], points[2] - points[1]));
          const auto origin = ray.origin + ray.direction * tmin;
          constexpr auto epsilon = 0.00000001;

          const auto edge1 = points[1] - points[0];
          const auto edge2 = points[2] - points[0];

          const auto P = cross(ray.direction, edge2);
          const double det = dot(P, edge1);

          if (det > epsilon) {
            Vector3 T = origin - points[0];
            double u = dot(P, T);

            if (u >= 0.0 && u <= 1.0 * det) {
              const auto Q = cross(T, edge1);
              const auto v = dot(Q, ray.direction);

              if (v >= 0.0 && (u + v) <= 1 * det) {
                double t = dot(Q, edge2) / det;
                auto hitpoint = origin + ray.direction * t;
                auto distance = sqrt((hitpoint - origin).norm());
                if (mindis > distance && tmax > distance) {
                  mindis = distance;
                  return Hit{distance, hitpoint, normal, this};
                }
              }
            }
          }

          index_offset += fnum;
        }
      }

      // 何も当たらなかった場合
      return min;
    }

    TriangleMesh::TriangleMesh(tinyobj::attrib_t &attrib, std::vector<tinyobj::shape_t> &shapes,
                               std::vector<tinyobj::material_t> &materials, Vector3 p, SurfaceType t, Vector3 color,
                               Vector3 em)
            :
            Object(p, t, color, em), attrib(attrib), shapes(shapes), materials(materials) {
    }
}