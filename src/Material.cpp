//
// Created by 長井亨 on 2018/07/23.
//

#include "Material.hpp"

namespace nagato
{
    Material::Material(SurfaceType t, Spectrum c) : surfaceType(t), color(c)
    {

    }

    SurfaceType Material::type()
    {
        return surfaceType;
    }
}
