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
            : surfaceType(t), color(c), refraction(Spectrum(1.5))
    {
        this->emitter = e * emitterL;
    }

    std::string Material::typeName()
    {
        std::string str;
        switch (type()) {
            case SurfaceType::Diffuse:
                str = "Diffuse";
                break;
            case SurfaceType::Mirror:
                str = "Mirror";
                break;
            case SurfaceType::Fresnel:
                str = "Fresnel";
                break;
            case SurfaceType::Emitter:
                str = "Emitter";
                break;
        }

        return str;
    }

    Material::Material(SurfaceType t, Spectrum c, Spectrum e, Spectrum refraction, float emitterL)
            : surfaceType(t), color(c), refraction(refraction)
    {

    }

    const Spectrum &Material::getRefraction() const
    {
        return refraction;
    }

}
