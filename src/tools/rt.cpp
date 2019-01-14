// Copyright (c) 2018 Mitsuba Rei
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <algorithm>
#include <random>
#include <cassert>
#include <tuple>
#include <omp.h>

#define UNREACHABLE() assert(0)

struct V {
    double x;
    double y;
    double z;
    V(double v = 0)
            : V(v, v, v) {}
    V(double x, double y, double z)
            : x(x), y(y), z(z) {}
    double operator[](int i) const {
        return (&x)[i];
    }
};

V operator+(V a, V b) {
    return V(a.x + b.x, a.y + b.y, a.z + b.z);
}
V operator-(V a, V b) {
    return V(a.x - b.x, a.y - b.y, a.z - b.z);
}
V operator*(V a, V b) {
    return V(a.x * b.x, a.y * b.y, a.z * b.z);
}
V operator/(V a, V b) {
    return V(a.x / b.x, a.y / b.y, a.z / b.z);
}
V operator-(V v) {
    return V(-v.x, -v.y, -v.z);
}

double dot(V a, V b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
V cross(V a, V b) {
    return V(a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x);
}
V normalize(V v) {
    return v / sqrt(dot(v, v));
}

std::tuple<V, V> tangentSpace(const V& n) {
    const double s = std::copysign(1, n.z);
    const double a = -1 / (s + n.z);
    const double b = n.x*n.y*a;
    return {
            V(1 + s * n.x*n.x*a,s*b,-s * n.x),
            V(b,s + n.y*n.y*a,-n.y)
    };
}

int tonemap(double v) {
    return std::min(
            std::max(int(std::pow(v, 1/2.2)*255), 0), 255);
};

struct Random {
    std::mt19937 engine;
    std::uniform_real_distribution<double> dist;
    Random() {};
    Random(int seed) {
        engine.seed(seed);
        dist.reset();
    }
    double next() { return dist(engine); }
};

struct Ray {
    V o;
    V d;
};

struct Sphere;
struct Hit {
    double t;
    V p;
    V n;
    const Sphere* sphere;
};

enum class SurfaceType {
    Diffuse,
    Mirror,
    Fresnel,
};

struct Sphere {
    V p;
    double r;
    SurfaceType type;
    V R;
    V Le;
    double ior = 1.5168;
    std::optional<Hit> intersect(
            const Ray& ray,
            double tmin,
            double tmax) const {
        const V op = p - ray.o;
        const double b = dot(op, ray.d);
        const double det = b * b - dot(op, op) + r*r;
        if (det < 0) { return {}; }
        const double t1 = b - sqrt(det);
        if (tmin<t1&&t1<tmax) { return Hit{t1, {}, {}, this}; }
        const double t2 = b + sqrt(det);
        if (tmin<t2&&t2<tmax) { return Hit{t2, {}, {}, this}; }
        return {};
    }
};

struct Scene {
    std::vector<Sphere> spheres{
            { V(1e5+1,40.8,81.6)  , 1e5 , SurfaceType::Diffuse, V(.75,.25,.25) },
            { V(-1e5+99,40.8,81.6), 1e5 , SurfaceType::Diffuse, V(.25,.25,.75) },
            { V(50,40.8,1e5)      , 1e5 , SurfaceType::Diffuse, V(.75) },
            { V(50,1e5,81.6)      , 1e5 , SurfaceType::Diffuse, V(.75) },
            { V(50,-1e5+81.6,81.6), 1e5 , SurfaceType::Diffuse, V(.75) },
            { V(27,16.5,47)       , 16.5, SurfaceType::Mirror, V(.999)  },
            { V(73,16.5,78)       , 16.5, SurfaceType::Fresnel, V(.999)  },
            { V(50,681.6-.27,81.6), 600 , SurfaceType::Diffuse, V(), V(12) },
    };
    std::optional<Hit> intersect(
            const Ray& ray,
            double tmin,
            double tmax) const {
        std::optional<Hit> minh;
        for (const auto& sphere : spheres) {
            const auto h = sphere.intersect(
                    ray, tmin, tmax);
            if (!h) { continue; }
            minh = h;
            tmax = minh->t;
        }
        if (minh) {
            const auto* s = minh->sphere;
            minh->p = ray.o + ray.d * minh->t;
            minh->n = (minh->p - s->p) / s->r;
        }
        return minh;
    }
};

int main() {
    // Image size
    const int w = 480;
    const int h = 360;

    // Samples per pixel
    const int spp = 1000;

    // Camera parameters
    const V eye(50, 52, 295.6);
    const V center = eye + V(0, -0.042612, -1);
    const V up(0, 1, 0);
    const double fov = 30 * M_PI / 180;
    const double aspect = double(w)/h;

    // Basis vectors for camera coordinates
    const auto wE = normalize(eye - center);
    const auto uE = normalize(cross(up, wE));
    const auto vE = cross(wE, uE);

    Scene scene;
    std::vector<V> I(w*h);
#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < w*h; i++) {
        thread_local Random rng(42 + omp_get_thread_num());
        for (int j = 0; j < spp; j++) {
            const int x = i % w;
            const int y = h - i / w;
            Ray ray;
            ray.o = eye;
            ray.d = [&]() {
                const double tf = std::tan(fov*.5);
                const double rpx = 2.*(x+rng.next())/w-1;
                const double rpy = 2.*(y+rng.next())/h-1;
                const V w = normalize(
                        V(aspect*tf*rpx, tf*rpy, -1));
                return uE*w.x + vE*w.y + wE*w.z;
            }();

            V L(0), th(1);
            for (int depth = 0; depth < 10; depth++) {
                // Intersection
                const auto h = scene.intersect(
                        ray, 1e-4, 1e+10);
                if (!h) {
                    break;
                }
                // Add contribution
                L = L + th * h->sphere->Le;
                // Update next direction
                ray.o = h->p;
                ray.d = [&]() {
                    if (h->sphere->type == SurfaceType::Diffuse) {
                        // Sample direction in local coordinates
                        const auto n = dot(h->n, -ray.d) > 0 ? h->n : -h->n;
                        const auto& [u, v] = tangentSpace(n);
                        const auto d = [&]() {
                            const double r = sqrt(rng.next());
                            const double t = 2*M_PI*rng.next();
                            const double x = r*cos(t);
                            const double y = r*sin(t);
                            return V(x, y,
                                     std::sqrt(
                                             std::max(.0,1-x*x-y*y)));
                        }();
                        // Convert to world coordinates
                        return u*d.x+v*d.y+n*d.z;
                    } else if (h->sphere->type == SurfaceType::Mirror) {
                        const auto wi = -ray.d;
                        return 2 * dot(wi, h->n) * h->n - wi;
                    } else if (h->sphere->type == SurfaceType::Fresnel) {
                        const auto wi = -ray.d;
                        const auto into = dot(wi, h->n) > 0;
                        const auto n = into ? h->n : -h->n;
                        const auto ior = h->sphere->ior;
                        const auto eta = into ? 1 / ior : ior;
                        const auto wt = [&]() -> std::optional<V> {
                            // Snell's law (vector form)
                            const auto t  = dot(wi, n);
                            const auto t2 = 1-eta*eta*(1-t*t);
                            if (t2 < 0) { return {}; }
                            return eta*(n*t-wi)-n*sqrt(t2);
                        }();
                        if (!wt) {
                            // Total internal reflection
                            return 2*dot(wi,h->n)*h->n-wi;
                        }
                        const auto Fr = [&]() {
                            // Schlick's approximation
                            const auto cos = into
                                             ? dot(wi, h->n)
                                             : dot(*wt, h->n);
                            const auto r = (1-ior)/(1+ior);
                            return r*r + (1-r*r)*pow(1-cos,5);
                        }();
                        // Select reflection or refraction
                        // according to the fresnel term
                        return rng.next() < Fr
                               ? 2*dot(wi, h->n)*h->n-wi
                               : *wt;
                    }
                    UNREACHABLE();
                    return V();
                }();
                // Update throughput
                th = th * h->sphere->R;
                if (std::max({ th.x,th.y,th.z }) == 0) {
                    break;
                }
            }
            I[i] = I[i] + L / spp;
        }
    }
    std::ofstream ofs("result.ppm");
    ofs << "P3\n" << w << " " << h << "\n255\n";
    for (const auto& i : I) {
        ofs << tonemap(i.x) << " "
            << tonemap(i.y) << " "
            << tonemap(i.z) << "\n";
    }
    return 0;
}