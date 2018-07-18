//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SPHERE_HPP
#define PATHTRACING_SPHERE_HPP


#include <optional>
#include "Common.hpp"
#include "Object.hpp"
#include "Hit.hpp"
#include "Ray.hpp"

namespace nagato {
    class Vector3;

    class Sphere : public Object {
    public:
        double radius;

        Sphere(Vector3 p, double r, SurfaceType t, Vector3 color, Vector3 em = Vector3());

        std::optional<Hit> intersect(
                Ray &ray,
                double tmin,
                double tmax) override;
    };
}
#endif //PATHTRACING_SPHERE_HPP
