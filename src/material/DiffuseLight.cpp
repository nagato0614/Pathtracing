//
// Created by 長井亨 on 2019/01/29.
//

#include "DiffuseLight.hpp"

namespace nagato {

    DiffuseLight::DiffuseLight(Spectrum e, float l)
    : Material(SurfaceType::Emitter,Spectrum(0), e / e.findMaxSpectrum(), l) {
        this->bsdf.reset(createBSDF(*this));

    }
}