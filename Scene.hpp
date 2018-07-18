//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SCENE_HPP
#define PATHTRACING_SCENE_HPP

#include "Object.hpp"
#include "Sphere.hpp"

namespace nagato {
    class Scene {
    public:

        std::vector<Object *> spheres{
                new Sphere{Vector3(-2, 1, 0), 1.1, SurfaceType::Mirror, Vector3(.999)},
                new Sphere{Vector3(2, 1, 0), 1.1, SurfaceType::Fresnel, Vector3(.999)},
                new Sphere{Vector3(0, 10, 0), 1, SurfaceType::Diffuse, Vector3(),
                           Vector3(50, 50, 50)},
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
          return minh;
        }
    };
}

#endif //PATHTRACING_SCENE_HPP
