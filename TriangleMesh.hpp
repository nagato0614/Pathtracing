//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_TRIANGLEMESH_HPP
#define PATHTRACING_TRIANGLEMESH_HPP


#include "tiny_obj_loader.h"
#include "Object.hpp"
#include "Common.hpp"
#include "Ray.hpp"
#include "Hit.hpp"

namespace nagato {

    class Object;
    class Hit;

    class Vector3;
    class TriangleMesh : public Object {
    public :

        TriangleMesh(
                tinyobj::attrib_t &attrib,
                std::vector<tinyobj::shape_t> &shapes,
                std::vector<tinyobj::material_t> &materials,
                Vector3 p,
                SurfaceType t,
                Spectrum color,
                Spectrum em = Spectrum());

        TriangleMesh(
                const std::string &objfilename,
                const std::string &mtlfilename,
                Vector3 p,
                SurfaceType t,
                Spectrum color,
                Spectrum em = Spectrum());

        std::optional<Hit> intersect(Ray &ray, float tmin, float tmax) override;

    private:
        tinyobj::attrib_t attrib{};
        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> materials{};
    };
}

#endif //PATHTRACING_TRIANGLEMESH_HPP
