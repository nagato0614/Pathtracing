#define _USE_MATH_DEFINES

// オブジェクトローダーの読み込み関連
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_DOUBLE

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <algorithm>
#include <random>
#include <tuple>
#include <omp.h>
#include <map>
#include <cfloat>
#include "Vector3.hpp"
#include "tiny_obj_loader.h"

using namespace nagato;

namespace nagato {
class Sphere;
class Object;
class Plane;
class Hit;

std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n) {
	const double s = std::copysign(1, n.z);
	const double a = -1 / (s + n.z);
	const double b = n.x * n.y * a;
	return {
			Vector3(1 + s * n.x * n.x * a, s * b, -s * n.x),
			Vector3(b, s + n.y * n.y * a, -n.y)
	};
}

class MaterialStringStreamReader : public tinyobj::MaterialReader {
 public:
	MaterialStringStreamReader(const std::string &matSStream)
			: m_matSStream(matSStream) {}
	virtual ~MaterialStringStreamReader() {}
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

int tonemap(double v) {
	return std::min(
			std::max(int(std::pow(v, 1 / 2.2) * 255), 0), 255);
};

int clamp(double v) {
	return std::min(std::max(0, int(v)), 255);
}

class Random {
 public:
	std::mt19937 engine;
	std::uniform_real_distribution<double> dist;
	Random() {};
	Random(int seed) {
		engine.seed(seed);
		dist.reset();
	}
	double next() { return dist(engine); }
};

class Ray {
 public:
	Vector3 origin;
	Vector3 direction;

//			Ray(Vector3 o, Vector3 dir)
//					: origin(o), direction(dir)
//			{}
};

class Hit {
 public:
	double distance;
	Vector3 point;
	Vector3 normal;
	Object *sphere;

	Hit(double d, Vector3 p, Vector3 n, Object *sphere)
			:
			distance(d), point(p), normal(n), sphere(sphere) {

	}
};

enum class SurfaceType {
	Diffuse,
	Mirror,
	Fresnel,
};

class Object {
 public:
	Vector3 position;
	SurfaceType type;
	Vector3 color;
	Vector3 emittance;
	double ior = 1.5;

	Object(Vector3 p, SurfaceType t, Vector3 color, Vector3 em = Vector3())
			: position(p), type(t), color(color), emittance(em) {}

	virtual std::optional<Hit> intersect(Ray &ray, double tmin, double tmax) {}
};

class Sphere : public Object {
 public:
	double radius;

	Sphere(Vector3 p, double r, SurfaceType t, Vector3 color, Vector3 em = Vector3())
			: Object(p, t, color, em), radius(r) {}

	std::optional<Hit> intersect(
			Ray &ray,
			double tmin,
			double tmax) override {
		const Vector3 op = position - ray.origin;
		const double b = dot(op, ray.direction);
		const double det = b * b - dot(op, op) + radius * radius;

		if (det < 0) {
			return {};
		}
		const double t1 = b - sqrt(det);

		if (tmin < t1 && t1 < tmax) {
			auto point = ray.origin + ray.direction * t1;
			auto normal = (point - position) / radius;
			return Hit{t1, point, normal, this};
		}

		const double t2 = b + sqrt(det);
		if (tmin < t2 && t2 < tmax) {
			auto point = ray.origin + ray.direction * t2;
			auto normal = (point - position) / radius;
			return Hit{t2, point, normal, this};
		}
		return {};
	}
};

class TriangleMesh : public Object {
 public :
	tinyobj::attrib_t &attrib;
	std::vector<tinyobj::shape_t> &shapes;
	std::vector<tinyobj::material_t> &materials;

	TriangleMesh(
			tinyobj::attrib_t &attrib,
			std::vector<tinyobj::shape_t> &shapes,
			std::vector<tinyobj::material_t> &materials,
			Vector3 p,
			SurfaceType t,
			Vector3 color,
			Vector3 em = Vector3()) :
			Object(p, t, color, em), attrib(attrib), shapes(shapes), materials(materials) {

	}

	std::optional<Hit> intersect(Ray &ray, double tmin, double tmax) override {

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

//					printf("(vertex, normal) = (%d, %d)\n", vertexIndex, normalIndex);
					auto vertex =
							Vector3(attrib.vertices[3 * vertexIndex],
											attrib.vertices[3 * vertexIndex + 1],
											attrib.vertices[3 * vertexIndex + 2]);
//					printVector3(vertex);
					points.push_back(vertex);

					// 法線情報がない場合はスキップ
					if (normalIndex >= 0) {
						auto normal =
								Vector3(attrib.normals[normalIndex], attrib.normals[normalIndex + 1], attrib.normals[normalIndex + 2]);
//						printVector3(normal);
						normals.push_back(normal);
					}
				}


				// 各ポリゴンに対する当たり判定
				const auto normal = normalize(cross(points[0] - points[1], points[1] - points[2]));
				const auto dotNoramlRay = dot(ray.origin + ray.direction * tmin - points[0], normal);
				const auto raydirNormal = dot(ray.direction, normal);

				// レイと平面が平行になっている
				if (raydirNormal == 0.0)
					continue;

				auto t = -dotNoramlRay / raydirNormal;

				// 視点の後方に平面が存在するか視点が平面に存在する
				if (t <= 0.0)
					continue;

				// レイと平面のヒットポイント(内部に存在するかどうかはまだわからない)
				auto hitPoint = ray.direction * t + ray.origin;

				std::vector<Vector3> z;
				for (int i = 0; i < points.size(); i++) {
					auto vv = points[(i + 1) % points.size()] - points[i % points.size()];
					auto pv = hitPoint - points[i % points.size()];

					auto cross_vv_pv = (cross(normalize(vv), normalize(pv)));
					z.push_back(normalize(cross_vv_pv));
				}

				// もしヒットしていた場合の処理
				if (dot(z[0], z[1]) > 0.0 && dot(z[1], z[2]) > 0.0) {
					double distance = sqrt((hitPoint - (ray.origin + ray.direction * tmin)).norm());
					if (mindis > distance && tmax > distance) {
//						std::cout << "hit" << std::endl;
						mindis = distance;
						min = Hit{distance, hitPoint, normal, this};
					}
				}

				index_offset += fnum;
			}
		}

		// 何も当たらなかった場合
		return min;
	}
};

class Plane : public Object {
 public:
	double edge;
	Vector3 normal;
	Vector3 point[4];
	Plane(Vector3 p, double e, Vector3 n, SurfaceType t, Vector3 color, Vector3 em = Vector3())
			: Object(p, t, color, em), edge(e), normal(n) {
		auto half = edge / 2.0;
		point[0] = Vector3{-half, 0, half};
		point[1] = Vector3{half, 0, half};
		point[2] = Vector3{half, 0, -half};
		point[3] = Vector3{-half, 0, -half};

		const auto trans = Matrix4::transform(position.x, position.y, position.z);
		const auto up = Vector3(0, 1, 0);
		const auto theta = acos(up.x * normal.x + up.y * normal.y);
		const auto phi = acos(up.x * normal.x + up.z * normal.z);
		const auto rot = Matrix4::rotate(2, theta) * Matrix4::rotate(1, phi);
		for (auto &i : point) {
			auto vec4 = toVec4(i);
			i = toVec3(trans * rot * vec4);
			printVector3(i);
		}
	}

	Plane(Vector3 p,
				Vector3 p0,
				Vector3 p1,
				Vector3 p2,
				Vector3 p3,
				Vector3 n,
				SurfaceType t,
				Vector3 color,
				Vector3 em = Vector3())
			: Object(p, t, color, em), normal(n) {
		point[0] = p0;
		point[1] = p1;
		point[2] = p2;
		point[3] = p3;
	}

	std::optional<Hit> intersect(Ray &ray, double tmin, double tmax) override {
		auto v1 = ray.origin + (ray.direction * tmin) - position;
		auto v2 = ray.origin + (ray.direction * tmax) - position;

		if (dot(v1, normal) * dot(v2, normal) <= 0.0) {
			// レイと平面が交わっている場合
//			auto p = point[0] - position;
//			auto pp = point[0] - point[0];
//			auto n = normalize(cross(p, pp));
//			for (int i = 1; i < 4; i++) {
//				p = point[i] - position;
//				pp = point[(i + 1) % 4] - point[i];
//				if (normalize(cross(p, pp)) != n) {
//					return {};
//				}
//			}

			auto hitpoint = ray.direction * (abs(dot(v1, normal)) / abs(dot(v1, normal) + dot(v2, normal)));
			auto dis = sqrt(dot(hitpoint - ray.origin,
													hitpoint - ray.origin));

			return Hit{dis, hitpoint, normal, this};
		}
		return {};
	}
};

class Scene {
 public:
//	std::vector<Object *> spheres{
//			new Sphere{Vector3(1e5 + 1, 40.8, 81.6), 1e5, SurfaceType::Diffuse,
//								 Vector3(.75, .25, .25)},
//			new Sphere{Vector3(-1e5 + 99, 40.8, 81.6), 1e5, SurfaceType::Diffuse,
//								 Vector3(.25, .25, .75)},
//			new Sphere{Vector3(50, 40.8, 1e5), 1e5, SurfaceType::Diffuse, Vector3(.75)},
//			new Sphere{Vector3(50, 1e5, 81.6), 1e5, SurfaceType::Diffuse, Vector3(.75)},
//			new Sphere{Vector3(50, -1e5 + 81.6, 81.6), 1e5, SurfaceType::Diffuse, Vector3(.75)},
//			new Sphere{Vector3(27, 16.5, 47), 16.5, SurfaceType::Mirror, Vector3(.999)},
//			new Sphere{Vector3(73, 16.5, 78), 16.5, SurfaceType::Fresnel, Vector3(.999)},
//			new Sphere{Vector3(50, 681.6 - .27, 81.6), 600, SurfaceType::Diffuse, Vector3(),
//								 Vector3(12)},
//	};

//	std::vector<Object *> spheres{
//			new Plane{Vector3(-3, 0, 0), 6, Vector3(1, 0, 0), SurfaceType::Diffuse,
//								Vector3(.75, .25, .25)},
//			new Plane{Vector3(3, 0, 0), 6, Vector3(-1, 0, 0), SurfaceType::Diffuse,
//								Vector3(.25, .25, .75)},
//			new Plane{Vector3(0, 3, 0), 6, Vector3(0, -1, 0), SurfaceType::Diffuse, Vector3(.75)},
//			new Plane{Vector3(0, -3, 0), 6, Vector3(0, 1, 0), SurfaceType::Diffuse, Vector3(.75)},
////			new Plane{Vector3(0, 0, -3), 6, Vector3(0, 0, 1), SurfaceType::Diffuse, Vector3(.75)},
//			new Sphere{Vector3(-1.5, -2, 0), 1, SurfaceType::Mirror, Vector3(.999)},
//			new Sphere{Vector3(1.5, -2, 0), 1, SurfaceType::Fresnel, Vector3(.999)},
//			new Sphere{Vector3(0, 3, 0), 0.5, SurfaceType::Diffuse, Vector3(), Vector3(12)}
//	};

//	std::vector<Object *> spheres{
//			new Plane{Vector3(3, 0, 0), Vector3(3, 3, 3), Vector3(3, 3, -3), Vector3(3, -3, -3), Vector3(3, -3, 3),
//								Vector3(-1, 0, 0), SurfaceType::Diffuse,
//								Vector3(.75, .25, .25)},
//			new Plane{Vector3(-3, 0, 0), Vector3(-3, 3, -3), Vector3(-3, 3, 3), Vector3(-3, -3, 3), Vector3(-3, -3, -3),
//								Vector3(1, 0, 0), SurfaceType::Diffuse,
//								Vector3(.25, .25, .75)},
//			new Plane{Vector3(0, 3, 0), Vector3(-3, 3, -3), Vector3(3, 3, -3), Vector3(3, 3, 3), Vector3(-3, 3, 3),
//								Vector3(0, -1, 0), SurfaceType::Diffuse, Vector3(.75)},
//			new Plane{Vector3(0, -3, 0), Vector3(-3, -3, -3), Vector3(3, -3, -3), Vector3(3, -3, 3), Vector3(-3, -3, 3),
//								Vector3(0, 1, 0), SurfaceType::Diffuse, Vector3(.75)},
//			new Plane{Vector3(0, 0, -3), Vector3(-3, 3, -3), Vector3(3, 3, -3), Vector3(3, -3, -3), Vector3(-3, -3, -3),
//								Vector3(0, 0, 1), SurfaceType::Diffuse, Vector3(.75)},
//			new Sphere{Vector3(-1.5, -2, 0), 1, SurfaceType::Mirror, Vector3(.999)},
//			new Sphere{Vector3(1.5, -2, 0), 1, SurfaceType::Fresnel, Vector3(.999)},
//			new Sphere{Vector3(0, 3, 0), 0.5, SurfaceType::Diffuse, Vector3(), Vector3(12)}
//	};

	std::vector<Object *> spheres{
			new Sphere{Vector3(-2.5, 1, 0),0.5, SurfaceType::Mirror, Vector3(.999)},
			new Sphere{Vector3(2.5, 1, 0), 0.5, SurfaceType::Fresnel, Vector3(.999)},
			new Sphere{Vector3(0, 8, 0), 0.5, SurfaceType::Diffuse, Vector3(),
								 Vector3(12)},
	};

	std::optional<Hit> intersect(Ray &ray, double tmin, double tmax) {
		std::optional<Hit> minh;
		for (auto &sphere : spheres) {
			auto h = sphere->intersect(ray, tmin, tmax);
			if (!h) {
				continue;
			}
			minh = h;
			tmax = minh->distance;
		}
//				if (minh) {
//					const auto *s = static_cast<Sphere *>(minh->sphere);
//					minh->point = ray.origin + ray.direction * minh->distance;
//					minh->normal = (minh->point - s->position) / s->radius;
//				}
		return minh;
	}
};
}

int main() {

	#ifdef _DEBUG
	std::cout << "-- DEBUF MODE --" << std::endl;
	#endif

	#ifndef _DEBUG
	std::cout << "-- REREASE MODE --" << std::endl;
	#endif

	// スレッド数の表示
	#ifdef _OPENMP
	std::cout << "-- openMP --" << std::endl;
	std::cout << "The number of processors is " << omp_get_num_procs() << std::endl;
	std::cout << "OpenMP : Enabled (Max # of threads = " << omp_get_max_threads() << ")" << std::endl;
	omp_set_num_threads(omp_get_max_threads());
	#endif

	std::cout << "-- Load Object File --" << std::endl;
	// オブジェクトファイルを読み込み
	std::ifstream cornellbox("./models/cornellbox_suzanne.obj");
	std::ifstream cornellob_material("./models/cornellbox_suzanne.mtl");


	if (cornellbox.fail()) {
		std::cerr << "オブジェクトファイルを開けませんでした" << std::endl;
		exit(-1);
	}
	if (cornellob_material.fail()) {
		std::cerr << "マテリアルファイルを開けませんでした" << std::endl;
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

	//　マテリアルローダを初期化
	MaterialStringStreamReader materialStringStreamReader(materialFileString);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool red = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &objectFileStream, &materialStringStreamReader);

	// Image size
	const int w = 460;
	const int h = 320;

	// Samples per pixel
	const int spp = 1;

	// Camera parameters
//	const Vector3 eye(50, 52, 295.6);
//	const Vector3 center = eye + Vector3(0, -0.042612, -1);

	const Vector3 eye(0, 5, 20);
	const Vector3 center = eye + Vector3(0, 0, -1);

	const Vector3 up(0, 1, 0);
	const double fov = 30 * M_PI / 180;
	const double aspect = double(w) / h;

	// Basis vectors for camera coordinates
	const auto wE = normalize(eye - center);
	const auto uE = normalize(cross(up, wE));
	const auto vE = cross(wE, uE);

	Scene scene;
	scene.spheres.push_back(new TriangleMesh(attrib,
																					 shapes,
																					 materials,
																					 Vector3(),
																					 SurfaceType::Diffuse,
																					 Vector3(0.3, 0.7, 0.3)));
	std::vector<Vector3> I(w * h);
	std::vector<Vector3> nom(w * h);
	std::vector<Vector3> depth_buffer(w * h);

	std::cout << "width : height = " << w << " : " << h << std::endl;
	std::cout << "number of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
	std::cout << "-- RENDERING START --" << std::endl;

	for (int pass = 0; pass < spp; pass++) {
		std::cout << "\rpath : " << (pass + 1) << " / " << spp;
		fflush(stdout);
#pragma omp parallel for schedule(dynamic, 1)
		for (int i = 0; i < w * h; i++) {
			thread_local Random rng(42 + omp_get_thread_num() + pass);
			const int x = i % w;
			const int y = h - i / w;
			Ray ray;
			ray.origin = eye;
			ray.direction = [&]() {
				const double tf = std::tan(fov * .5);
				const double rpx = 2. * (x + rng.next()) / w - 1;
				const double rpy = 2. * (y + rng.next()) / h - 1;
				const Vector3 w = normalize(
						Vector3(aspect * tf * rpx, tf * rpy, -1));
				return uE * w.x + vE * w.y + wE * w.z;
			}();

			Vector3 L(0), th(1);
			for (int depth = 0; depth < 1; depth++) {
				// Intersection
				const auto intersect = scene.intersect(
						ray, 1e-4, 1e+100);
				if (!intersect) {
					break;
				}

				if (pass == 0 && depth == 0) {
					nom[i] = (normalize(intersect->normal) + 1.0) / 2.0 * 255;
					auto d = intersect->distance;
					depth_buffer[i] = {d, d, d};
				}
				// Add contribution
				L = L + th * intersect->sphere->emittance;
				// Update next direction
				ray.origin = intersect->point;
				ray.direction = [&]() {
					if (intersect->sphere->type == SurfaceType::Diffuse) {
						// Sample direction in local coordinates
						const auto n = dot(intersect->normal, -ray.direction) > 0 ? intersect->normal : -intersect->normal;
						const auto&[u, v] = tangentSpace(n);
						const auto d = [&]() {
							const double r = sqrt(rng.next());
							const double t = 2 * M_PI * rng.next();
							const double x = r * cos(t);
							const double y = r * sin(t);
							return Vector3(x, y,
														 std::sqrt(
																 std::max(.0, 1 - x * x - y * y)));
						}();
						// Convert to world coordinates
						return u * d.x + v * d.y + n * d.z;
					} else if (intersect->sphere->type == SurfaceType::Mirror) {
						const auto wi = -ray.direction;
						return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
					} else if (intersect->sphere->type == SurfaceType::Fresnel) {
						const auto wi = -ray.direction;
						const auto into = dot(wi, intersect->normal) > 0;
						const auto n = into ? intersect->normal : -intersect->normal;
						const auto ior = intersect->sphere->ior;
						const auto eta = into ? 1 / ior : ior;
						const auto wt = [&]() -> std::optional<Vector3> {
							const auto t = dot(wi, n);
							const auto t2 = 1 - eta * eta * (1 - t * t);
							if (t2 < 0) {
								return {};
							};
							return (n * t - wi) * eta - n * sqrt(t2);
						}();
						if (!wt) {
							return intersect->normal * 2 * dot(wi, intersect->normal) - wi;
						}
						const auto Fr = [&]() {
							const auto cos = into ? dot(wi, intersect->normal) : dot(*wt, intersect->normal);
							const auto r = (1 - ior) / (1 + ior);
							return r * r + (1 - r * r) * pow(1 - cos, 5);
						}();

						return rng.next() < Fr ? intersect->normal * 2 * dot(wi, intersect->normal) * intersect->normal - wi : *wt;
					}
				}();

				// Update throughput
				th = th * intersect->sphere->color;
				if (std::max({th.x, th.y, th.z}) == 0) {
					break;
				}
			}
			I[i] = I[i] + L / spp;
		}
	}
	std::ofstream ofs("result.ppm");
	ofs << "P3\n" << w << " " << h << "\n255\n";
	for (const auto &i : I) {
		ofs << tonemap(i.x) << " "
				<< tonemap(i.y) << " "
				<< tonemap(i.z) << "\n";
	}

	// 法線マップを出力
	std::ofstream output_normal("normal.ppm");
	output_normal << "P3\n" << w << " " << h << "\n255\n";
	for (const auto &i : nom) {
		output_normal << clamp(i.x) << " "
									<< clamp(i.y)  << " "
									<< clamp(i.z)  << "\n";
	}

	double max = 0;

	for (const auto &i : depth_buffer) {
		if (i.x > max) {
			max = i.x;
		}
	}

	for (auto &i : depth_buffer) {
		i = i / max;
	}

	// デプスを出力
	std::ofstream output_depth("depth.ppm");
	output_depth << "P3\n" << w << " " << h << "\n255\n";
	for (const auto &i : depth_buffer) {
		output_depth << tonemap(i.x) << " "
								 << tonemap(i.y) << " "
								 << tonemap(i.z) << "\n";
	}
	std::cout << "\n-- FINISH --" << std::endl;
	return 0;
}
