//
// Created by 長井亨 on 2018/07/18.
//

#include "Scene.hpp"
#include "Object.hpp"

namespace nagato {

    std::optional<Hit> Scene::intersect(Ray &ray, double tmin, double tmax) {
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
}