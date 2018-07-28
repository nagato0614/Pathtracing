//
// Created by 長井亨 on 2018/07/18.
//

#include "Scene.hpp"
#include "Object.hpp"
#include "Hit.hpp"

namespace nagato {

    std::optional<Hit> Scene::intersect(Ray &ray, float tmin, float tmax) {
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
}
