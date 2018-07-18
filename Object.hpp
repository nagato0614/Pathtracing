//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_OBJECT_HPP
#define PATHTRACING_OBJECT_HPP


#include <optional>
#include "Ray.hpp"
#include "SurfaceType.hpp"
#include "Vector3.hpp"

namespace nagato {

    class Hit;

    class Object {
    public:
        Vector3 position;
        SurfaceType type;
        Vector3 color;
        Vector3 emittance;
        double ior = 1.5;

        Object(Vector3 p, SurfaceType t, Vector3 color, Vector3 em = Vector3());

        virtual std::optional<Hit> intersect(Ray &ray, double tmin, double tmax);
    };
}

#endif //PATHTRACING_OBJECT_HPP
