//
// Created by 長井亨 on 2018/09/06.
//

#include "Specular.hpp"

namespace nagato {

    Specular::Specular(Material *m) : BSDF(m)
    {

    }

    Spectrum Specular::makeNewDirection(int *wavelengthIndex, Vector3 *newDirection, Ray &ray, Hit &surfaceInfo)
    {
        const auto wi = -ray.direction;
        *newDirection = surfaceInfo.normal * 2 * dot(wi, surfaceInfo.normal) - wi;
        return material->color;
    }
}