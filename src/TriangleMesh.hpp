//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_TRIANGLEMESH_HPP
#define PATHTRACING_TRIANGLEMESH_HPP


#include "tiny_obj_loader.h"
#include "SurfaceType.hpp"
#include "Spectrum.hpp"
#include "Ray.hpp"
#include "Object.hpp"

namespace nagato {

    class TriangleMesh : public Object {
    public :

        TriangleMesh(
                tinyobj::attrib_t &attrib,
                std::vector<tinyobj::shape_t> &shapes,
                std::vector<tinyobj::material_t> &materials,
                Material *m);

        TriangleMesh(
                const std::string &objfilename,
                const std::string &mtlfilename,
                Material *m);

        std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);

    private:
        tinyobj::attrib_t attrib{};
        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> materials{};
    };
}

#endif //PATHTRACING_TRIANGLEMESH_HPP
