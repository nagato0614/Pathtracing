//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SCENE_HPP
#define PATHTRACING_SCENE_HPP


#include <optional>
#include <vector>
#include "../object/Object.hpp"

namespace nagato {
    class Scene {
    public:
        Scene();

        std::vector<Object *> objects;

        std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);

        void loadObject(const std::string &objfilename,
                        const std::string &mtlfilename,
                        Material *m);

        void freeObject();
    };
}

#endif //PATHTRACING_SCENE_HPP
