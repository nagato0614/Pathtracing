//
// Created by 長井亨 on 2019/01/29.
//

#include "Diffuse.hpp"

namespace nagato {
    Diffuse::Diffuse(nagato::Spectrum c) : Material(SurfaceType::Diffuse, c, Spectrum(), 1) {
        this->bsdf.reset(createBSDF(*this));

    }
}