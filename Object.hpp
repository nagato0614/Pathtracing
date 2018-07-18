//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_OBJECT_HPP
#define PATHTRACING_OBJECT_HPP

#include "Vector3.hpp"
#include "Hit.hpp"
#include "Ray.hpp"
#include "Common.hpp"


namespace nagato {

    class Object {
    public:
        Vector3 position;
        SurfaceType type;
        Vector3 color;
        Vector3 emittance;
        double ior = 1.5;

        Object(Vector3 p, SurfaceType t, Vector3 color, Vector3 em = Vector3())
                : position(p), type(t), color(color), emittance(em) {}

        virtual std::optional<Hit> intersect(Ray &ray, double tmin, double tmax) {
            return {};
        }
    };
}

#endif //PATHTRACING_OBJECT_HPP
