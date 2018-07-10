#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <algorithm>
#include <random>
#include <tuple>
#include <omp.h>
#include "Vector3.hpp"

using namespace nagato;

namespace nagato
{
	class Object;
	class Sphere;
	class Hit;

	std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n)
	{
		const double s = std::copysign(1, n.z);
		const double a = -1 / (s + n.z);
		const double b = n.x * n.y * a;
		return {
				Vector3(1 + s * n.x * n.x * a, s * b, -s * n.x),
				Vector3(b, s + n.y * n.y * a, -n.y)
		};
	}

	int tonemap(double v)
	{
		return std::min(
				std::max(int(std::pow(v, 1 / 2.2) * 255), 0), 255);
	};

	class Random
	{
	public:
			std::mt19937 engine;
			std::uniform_real_distribution<double> dist;
			Random()
			{};
			Random(int seed)
			{
				engine.seed(seed);
				dist.reset();
			}
			double next()
			{ return dist(engine); }
	};

	class Ray
	{
	public:
			Vector3 origin;
			Vector3 direction;

//			Ray(Vector3 o, Vector3 dir)
//					: origin(o), direction(dir)
//			{}
	};



	class Hit
	{
	public:
			double distance;
			Vector3 point;
			Vector3 normal;
			Object *sphere;

			Hit(double d, Vector3 p, Vector3 n, Object *sphere)
					:
					distance(d), point(p), normal(n), sphere(sphere)
			{

			}
	};

	enum class SurfaceType
	{
			Diffuse,
			Mirror,
			Fresnel,
	};

	class Object
	{
	public:
			Vector3 position;
			SurfaceType type;
			Vector3 color;
			Vector3 emittance;
			double ior = 1.5;

			Object(Vector3 p, SurfaceType t, Vector3 color, Vector3 em = Vector3())
					: position(p), type(t), color(color), emittance(em)
			{}

//			virtual std::optional<Hit> intersect(Ray &ray, double tmin, double tmax);
	};

	class Sphere: public Object
	{
	public:
			double radius;

			Sphere(Vector3 p, double r, SurfaceType t, Vector3 color, Vector3 em = Vector3())
					: Object(p, t, color, em), radius(r)
			{}

			std::optional<Hit> intersect(
					Ray &ray,
					double tmin,
					double tmax)
			{
				const Vector3 op = position - ray.origin;
				const double b = dot(op, ray.direction);
				const double det = b * b - dot(op, op) + radius * radius;
				if (det < 0) { return {}; }
				const double t1 = b - sqrt(det);
				if (tmin < t1 && t1 < tmax) { return Hit{t1, {}, {}, this}; }
				const double t2 = b + sqrt(det);
				if (tmin < t2 && t2 < tmax) { return Hit{t2, {}, {}, this}; }
				return {};
			}
	};

	class Plane: public Object
	{
	public:
			double edge;
			Vector3 normal;
			Plane(Vector3 p, double e, Vector3 n, SurfaceType t, Vector3 color, Vector3 em = Vector3())
					: Object(p, t, color, em), edge(e), normal(n)
			{}

			std::optional<Hit> intersect(Ray &ray, double tmin, double tmax)
			{
				const Vector3 s = ray.origin - position;
				const auto sn = dot(s, normal);
				const auto dn = dot(ray.direction, normal);

				if (dn == 0.0) {
					return  {};
				}

				const auto t = -(sn / dn);
				if (t > 0.0)
				{
					return Hit{t, {}, {}, this};
				}
				else if (t == 0.0)
				{
					return {};
				}
				else if (t < 0.0) {
					return {};
				}
			}
	};

	class Scene
	{
	public:
			std::vector<Sphere> spheres{
					Sphere{Vector3(1e5 + 1, 40.8, 81.6), 1e5, SurfaceType::Diffuse, Vector3(.75, .25, .25)},
					Sphere{Vector3(-1e5 + 99, 40.8, 81.6), 1e5, SurfaceType::Diffuse, Vector3(.25, .25, .75)},
					Sphere{Vector3(50, 40.8, 1e5), 1e5, SurfaceType::Diffuse, Vector3(.75)},
					Sphere{Vector3(50, 1e5, 81.6), 1e5, SurfaceType::Diffuse, Vector3(.75)},
					Sphere{Vector3(50, -1e5 + 81.6, 81.6), 1e5, SurfaceType::Diffuse, Vector3(.75)},
					Sphere{Vector3(27, 16.5, 47), 16.5, SurfaceType::Mirror, Vector3(.999)},
					Sphere{Vector3(73, 16.5, 78), 16.5, SurfaceType::Fresnel, Vector3(.999)},
					Sphere{Vector3(50, 681.6 - .27, 81.6), 600, SurfaceType::Diffuse, Vector3(), Vector3(12)},
			};
			std::optional<Hit> intersect(Ray &ray, double tmin, double tmax)
			{
				std::optional<Hit> minh;
				for (auto &sphere : spheres) {
					auto h = sphere.intersect(ray, tmin, tmax);
					if (!h) { continue; }
					minh = h;
					tmax = minh->distance;
				}
				if (minh) {
					const auto *s = static_cast<Sphere *>(minh->sphere);
					minh->point = ray.origin + ray.direction * minh->distance;
					minh->normal = (minh->point - s->position) / s->radius;
				}
				return minh;
			}
	};
}

int main()
{
	// Image size
	const int w = 460;
	const int h = 320;

	// Samples per pixel
	const int spp = 10;

	// Camera parameters
	const Vector3 eye(50, 52, 295.6);
	const Vector3 center = eye + Vector3(0, -0.042612, -1);
	const Vector3 up(0, 1, 0);
	const double fov = 30 * M_PI / 180;
	const double aspect = double(w) / h;

	// Basis vectors for camera coordinates
	const auto wE = normalize(eye - center);
	const auto uE = normalize(cross(up, wE));
	const auto vE = cross(wE, uE);

	Scene scene;
	std::vector<Vector3> I(w * h);
	for (int j = 0; j < spp; j++) {
		std::cout << "\rpash : " << j;
		fflush(stdout);
#pragma omp parallel for schedule(dynamic, 1)
		for (int i = 0; i < w * h; i++) {
			thread_local Random rng(42 + omp_get_thread_num() + j);
			const int x = i % w;
			const int y = h - i / w;
			Ray ray;
			ray.origin = eye;
			ray.direction = [&]()
			{
					const double tf = std::tan(fov * .5);
					const double rpx = 2. * (x + rng.next()) / w - 1;
					const double rpy = 2. * (y + rng.next()) / h - 1;
					const Vector3 w = normalize(
							Vector3(aspect * tf * rpx, tf * rpy, -1));
					return uE * w.x + vE * w.y + wE * w.z;
			}();

			Vector3 L(0), th(1);
			for (int depth = 0; depth < 10; depth++) {
				// Intersection
				const auto h = scene.intersect(
						ray, 1e-4, 1e+10);
				if (!h) {
					break;
				}
				// Add contribution
				L = L + th * h->sphere->emittance;
				// Update next direction
				ray.origin = h->point;
				ray.direction = [&]()
				{
						if (h->sphere->type == SurfaceType::Diffuse) {
							// Sample direction in local coordinates
							const auto n = dot(h->normal, -ray.direction) > 0 ? h->normal : -h->normal;
							const auto&[u, v] = tangentSpace(n);
							const auto d = [&]()
							{
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
						}
						else if (h->sphere->type == SurfaceType::Mirror) {
							const auto wi = -ray.direction;
							return h->normal * 2 * dot(wi, h->normal) - wi;
						}
						else if (h->sphere->type == SurfaceType::Fresnel) {
							const auto wi = -ray.direction;
							const auto into = dot(wi, h->normal) > 0;
							const auto n = into ? h->normal : -h->normal;
							const auto ior = h->sphere->ior;
							const auto eta = into ? 1 / ior : ior;
							const auto wt = [&]() -> std::optional<Vector3>
							{
									const auto t = dot(wi, n);
									const auto t2 = 1 - eta * eta * (1 - t * t);
									if (t2 < 0) { return {}; };
									return (n * t - wi) * eta - n * sqrt(t2);
							}();
							if (!wt) {
								return h->normal * 2 * dot(wi, h->normal) - wi;
							}
							const auto Fr = [&]()
							{
									const auto cos = into ? dot(wi, h->normal) : dot(*wt, h->normal);
									const auto r = (1 - ior) / (1 + ior);
									return r * r + (1 - r * r) * pow(1 - cos, 5);
							}();

							return rng.next() < Fr ? h->normal * 2 * dot(wi, h->normal) * h->normal - wi : *wt;
						}
				}();

				// Update throughput
				th = th * h->sphere->color;
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

	std::cout << "\nFINISH" << std::endl;
	return 0;
}
