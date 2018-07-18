//
// Created by 長井亨 on 2018/07/18.
//

#include "Object.hpp"
#include "Hit.hpp"
#include "SurfaceType.hpp"

namespace nagato {

    Object::Object(Vector3 p, SurfaceType t, Vector3 color, Vector3 em)
            : position(p), type(t), color(color), emittance(em) {}

    std::optional<Hit> Object::intersect(Ray &ray, double tmin, double tmax) {
      return {};
    }
}