//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_OBJECT_HPP
#define PATHTRACING_OBJECT_HPP


#include <optional>
#include "Ray.hpp"
#include "Spectrum.hpp"
#include "SurfaceType.hpp"
#include "Hit.hpp"
#include "Material.hpp"

namespace nagato {

    class Hit;
    class Object {
    public:
        Material *material;
        float ior = 1.5;

        explicit Object(Material *m);

        virtual std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);
    };
}

#endif //PATHTRACING_OBJECT_HPP
