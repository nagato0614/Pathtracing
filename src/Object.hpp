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

namespace nagato {

    class Hit;
    class Object {
    public:
        Vector3 position;
        SurfaceType type;
        Spectrum color;
        Spectrum emittance;
        float ior = 1.5;

        Object(SurfaceType t, Spectrum color, Spectrum em = Spectrum());

        virtual std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);
    };
}

#endif //PATHTRACING_OBJECT_HPP
