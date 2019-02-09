//
// Created by 長井亨 on 2019-02-06.
//

#include "SimpleSky.hpp"

namespace nagato {

    Spectrum SimpleSky::getRadiance(const Ray &ray) const {
        float t = (ray.getDirection().y + 1 ) / 2.0f;
        return (1 - t) * Spectrum(1) + t * skyColor;
    }

    SimpleSky::SimpleSky(const Spectrum &s) : skyColor(s) {

    }
}