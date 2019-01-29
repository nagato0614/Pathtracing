//
// Created by 長井亨 on 2019/01/29.
//

#include "Glass.hpp"
#include "../BSDF/Fresnel.hpp"

namespace nagato {

    Glass::Glass(Spectrum c, float ior)
    : Material(SurfaceType::Fresnel, c, Spectrum(), 0), ior(ior) {
        this->bsdf = createFresnel(c, ior);
    }
}