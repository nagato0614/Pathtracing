//
// Created by 長井亨 on 2018/07/23.
//

#include "Material.hpp"
#include <utility>

namespace nagato {


    SurfaceType Material::type() const {
        return surfaceType;
    }

    Material::Material(SurfaceType t, Spectrum c, Spectrum e, float emitterL)
            : surfaceType(t), color(std::move(c)), refraction(Spectrum(1.5)) {
        this->emitter = e * emitterL;
        this->bsdf.reset(createBSDF(*this));
    }

    std::string Material::typeName() {
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

    const Spectrum &Material::getRefraction() const {
        return refraction;
    }

    BSDF &Material::getBSDF() {
        return *bsdf;
    }

    void Material::setRefraction(const Spectrum &refraction) {
        Material::refraction = refraction;
    }

    Material::~Material() {
    }
}
