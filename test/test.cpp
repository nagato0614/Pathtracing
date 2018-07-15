//
// Created by 長井亨 on 2018/07/11.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include "../Matrix4.hpp"
#include "../Vector4.hpp"
#include "../Vector3.hpp"
#include "../tiny_obj_loader.h"

using namespace nagato;

class MaterialStringStreamReader : public tinyobj::MaterialReader {
 public:
	explicit MaterialStringStreamReader(const std::string &matSStream)
			: m_matSStream(matSStream) {}
	~MaterialStringStreamReader() override {}
	virtual bool operator()(const std::string &matId,
													std::vector<tinyobj::material_t> *materials,
													std::map<std::string, int> *matMap,
													std::string *err) {
		(void) matId;
		std::string warning;
		tinyobj::LoadMtl(matMap, materials, &m_matSStream, &warning);

		if (!warning.empty()) {
			if (err) {
				(*err) += warning;
			}
		}
		return true;
	}

 private:
	std::stringstream m_matSStream;
};

int main(void) {
	std::cout << "TEST MATRIX" << std::endl;
//
//	double d[4][4] = {
//		{ 7, 9, 3, 8 },
//		{ 0, 2, 4, 8 },
//		{ 3, 9, 0, 5 },
//		{ 2, 2, 7, 3 }
//	};
//
//	Matrix4 d1(d);
//
//	printMatrix4(Matrix4::identity());
//	printMatrix4(d1);
//
//	auto d2 = d1 * d1;
//
//	printMatrix4(d2);
//
//	auto d3 = d1 + d1;
//	auto d4 = d1 - d1;
//
//	printMatrix4(d3);
//	printMatrix4(d4);
//
//	Vector4 v(1, 2, 3, 4);
//
//	printVector4(v);
//
//	auto v1 = v * d1;
//	auto v2 = d1 * v;
//
//	printVector4(v1);
//	printVector4(v2);
//
//	double edge = 6;
//	auto position = Vector3(0, 0, -3);
//	auto normal = Vector3(0, 0, 1);
//	Vector3 point[4];
//	auto half = edge / 2.0;
//	point[0] = Vector3{-half, 0, half};
//	point[1] = Vector3{half, 0, half};
//	point[2] = Vector3{half, 0, -half};
//	point[3] = Vector3{-half, 0, -half};
//
//	for (const auto &i : point)
//		printVector3(i);
//
//	std::cout << "--------------------------------" << std::endl;
//
//	const auto trans = Matrix4::transform(position.x, position.y, position.z);
//	printMatrix4(trans);
//
//	const auto up = Vector3(0, 1, 0);
//
//	const auto theta = acos(up.x * normal.x + up.y * normal.y);
//	const auto phi = acos(up.x * normal.x + up.z * normal.z);
//
//	std::cout << "--------------------------------" << std::endl;
//	std::cout << theta << " " << phi << std::endl;
//	std::cout << "--------------------------------" << std::endl;
//
//	// z軸中心の回転
//	const auto theta_m =  Matrix4::rotate(2, theta);
//
//	// x軸中心の回転
//	const auto phi_m =  Matrix4::rotate(1, phi);
//
//	printMatrix4(theta_m);
//	printMatrix4(phi_m);
//
//	const auto rot = theta_m * phi_m;
//
//	for (auto &i : point) {
//		auto vec4 = toVec4(i, 1);
//		i = toVec3(trans * rot * vec4);
//		printVector3(i);
//	}



	// オブジェクトファイルを読み込み
//	std::ifstream objctFile("./models/cornellbox_suzanne.obj");
//	std::ifstream materialFile("./models/cornellbox_suzanne.mtl");
//
//	if (objctFile.fail()) {
//		std::cerr << "オブジェクトファイルを開けませんでした" << std::endl;
//		exit(-1);
//	}
//	if (materialFile.fail()) {
//		std::cerr << "マテリアルファイルを開けませんでした" << std::endl;
//		exit(-1);
//	}
//
//	// オブジェクトファイルを文字列に変換
//	std::stringstream objectFileStream;
//	objectFileStream << objctFile.rdbuf();
//	objctFile.close();
//
//	std::stringstream strstream;
//	strstream.str("");
//	strstream << materialFile.rdbuf();
//	materialFile.close();
//	std::string materialFileString(strstream.str());
//
//
//	//　マテリアルローダを初期化
//	MaterialStringStreamReader materialStringStreamReader(materialFileString);
//
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string err;
//	bool red = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &objectFileStream, &materialStringStreamReader);
//
//	// すべてのポリゴンに対して辺り判定を行う(線形探索)
//	// その中で最も近いものをレイの交点とする
//	for (auto &shape : shapes) {
//		size_t index_offset = 0;
//		auto shapeNumber = shape.mesh.num_face_vertices;
//		std::vector<Vector3> points;
//		std::vector<Vector3> normals;
//
//		std::cout << "-------------- " << shape.name << " ------------------" << std::endl;
//
//		// .objファイルに含まれている各オブジェクトに対しての処理
//		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
//			size_t fnum = shape.mesh.num_face_vertices[f];
//
//			points.clear();
//			normals.clear();
//
//			std::cout << "-------------- " << fnum << " ------------------" << std::endl;
//			// 各面の頂点に対する処理
//			for (size_t v = 0; v < fnum; v++) {
//
//				// 各面の頂点情報と法線情報の取得
//				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
//				auto vertexIndex = idx.vertex_index;
//				auto normalIndex = idx.normal_index;
//
//				printf("(vertex, normal) = (%d, %d)\n", vertexIndex, normalIndex);
//				auto vertex =
//						Vector3(attrib.vertices[3 * vertexIndex],
//										attrib.vertices[3 * vertexIndex + 1],
//										attrib.vertices[3 * vertexIndex + 2]);
//				printVector3(vertex);
//				points.push_back(vertex);
//
//				// 法線情報がない場合はスキップcmaek
//				if (normalIndex >= 0) {
//					auto normal =
//							Vector3(attrib.normals[normalIndex], attrib.normals[normalIndex + 1], attrib.normals[normalIndex + 2]);
//					printVector3(normal);
//					normals.push_back(normal);
//				}
//			}
//			index_offset += fnum;
//		}
//	}

	Vector3 points[3] = {
			{0, 1, 0},
			{1, 0, 0},
			{-1, 0, 0},
	};

	Vector3 rayorigin(0, 0.5, 1);
	Vector3 raydirection(0, 0,-1);
	double tmin = 1e-4;
	double tmax = 1e+10;

	auto normal = normalize(cross(points[1] - points[0], points[2] - points[1]));
	std::cout << "-- noraml --" << std::endl;
	printVector3(normal);


	auto dotNormalRay = dot(rayorigin + raydirection * tmin - points[0], normal);
	auto raydirNormal = dot(raydirection, normal);

	if (raydirNormal == 0.0) {
		std::cout << "並行" << std::endl;
		exit(0);
	}

	auto t = -dotNormalRay / raydirNormal;
	if (t <= 0) {
		std::cout << "視点が平面" << std::endl;
		exit(0);
	}

	auto hitPoint = raydirection * t + rayorigin;
	std::cout << "-- hitpoint --" << std::endl;
	printVector3(hitPoint);

	int flag = 0;
	for (int i = 0; i < 3; i++) {
		auto vv = points[(i + 1) % 3] - points[i % 3];
		auto pv = hitPoint - points[i % 3];

		if (cross(vv ,pv).norm() < 0.0)
			flag++;
	}

	std::cout << "flag : " << flag << std::endl;
}
