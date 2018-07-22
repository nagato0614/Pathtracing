//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SCENE_HPP
#define PATHTRACING_SCENE_HPP


#include <optional>
#include <vector>
#include "Hit.hpp"
#include "Ray.hpp"

namespace nagato {
    class Scene {
    public:

        std::vector<Object *> spheres;

        std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);
    };
}

#endif //PATHTRACING_SCENE_HPP
