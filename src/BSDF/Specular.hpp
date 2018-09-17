//
// Created by 長井亨 on 2018/09/06.
//

#ifndef PATHTRACING_SPECULAR_HPP
#define PATHTRACING_SPECULAR_HPP

#include "BSDF.hpp"

namespace nagato
{
    class Specular : public BSDF
    {
     public:
        Specular(Material *m);

        Spectrum makeNewDirection(
                int *wavelengthIndex,
                Vector3 *newDirection,
                Ray &ray,
                const Hit &surfaceInfo) const override;
    };
}

#endif //PATHTRACING_SPECULAR_HPP
