//
// Created by 長井亨 on 2018/09/06.
//

#include "Specular.hpp"

namespace nagato {

    Specular::Specular(Spectrum c) : BSDF(c) {

    }

    Spectrum Specular::makeNewDirection(
            int *wavelengthIndex,
            Vector3 *newDirection,
            Ray &ray,
            const Hit &surfaceInfo) const {
        const auto wi = -ray.direction;
        *newDirection = surfaceInfo.getNormal() * 2 * dot(wi, surfaceInfo.getNormal()) - wi;
        return this->color;
    }
}