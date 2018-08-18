//
// Created by 長井亨 on 2018/07/23.
//

#include "Material.hpp"

namespace nagato
{


    SurfaceType Material::type()
    {
        return surfaceType;
    }

    Material::Material(SurfaceType t, Spectrum c, Spectrum e, float emitterL)
        : surfaceType(t), color(c)
    {
        this->emitter = e * emitterL;
    }

}
