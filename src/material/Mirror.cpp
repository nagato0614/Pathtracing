//
// Created by 長井亨 on 2019/01/29.
//

#include "Mirror.hpp"

namespace nagato{

    Mirror::Mirror(Spectrum c)
    : Material(SurfaceType::Mirror, c, Spectrum(), 0) {
        this->bsdf.reset(createBSDF(*this));
    }
}