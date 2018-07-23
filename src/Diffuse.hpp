//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_DIFFUSE_HPP
#define PATHTRACING_DIFFUSE_HPP

#include "Material.hpp"

namespace nagato
{
    class Diffuse :public Material
    {
     public:
        Diffuse(SurfaceType t, Spectrum c);
    };
}

#endif //PATHTRACING_DIFFUSE_HPP
