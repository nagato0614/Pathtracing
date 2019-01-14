//
// Created by 長井亨 on 2018/09/06.
//

#include "Fresnel.hpp"

namespace nagato {

    Fresnel::Fresnel(Spectrum c) : BSDF(c) {

    }

    Spectrum Fresnel::makeNewDirection(
            int *wavelengthIndex,
            Vector3 *newDirection,
            Ray &ray,
            const Hit &surfaceInfo) const {
        // #TODO : 波長ごとに異なる屈折率を扱えるようにする
        float ior = 1.5;

        const auto wi = -ray.getDirection();
        const auto into = dot(wi, surfaceInfo.getNormal()) > 0;
        const auto n = into ? surfaceInfo.getNormal() : -surfaceInfo.getNormal();
        const auto eta = into ? 1 / ior : ior;
        const auto wt = [&]() -> std::optional<Vector3> {
            const auto t = dot(wi, n);
            const auto t2 = 1 - eta * eta * (1 - t * t);
            if (t2 < 0) {
                return {};
            };
            return (n * t - wi) * eta - n * sqrt(t2);
        }();
        if (!wt) {
            *newDirection = surfaceInfo.getNormal() * 2 * dot(wi, surfaceInfo.getNormal()) - wi;
            return this->color;
        }
        const auto Fr = [&]() {
            const auto cos = into ? dot(wi, surfaceInfo.getNormal()) : dot(*wt, surfaceInfo.getNormal());
            const auto r = (1 - ior) / (1 + ior);
            return r * r + (1 - r * r) * pow(1 - cos, 5);
        }();

        if (Random::Instance().next() < Fr) {
            *newDirection = surfaceInfo.getNormal() * 2 * dot(wi, surfaceInfo.getNormal()) - wi;
            return this->color * Fr;
        } else {
            *newDirection = *wt;
            return this->color * (1 - Fr);
        }
    }
}