//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_SPECULAR_HPP
#define PATHTRACING_SPECULAR_HPP

#include "Material.hpp"

namespace nagato
{
    class Specular : public Material
    {
     public:
        Specular(SurfaceType t, Spectrum c);
    };
}

#endif //PATHTRACING_SPECULAR_HPP
