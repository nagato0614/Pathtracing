//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_EMITTER_HPP
#define PATHTRACING_EMITTER_HPP

#include "Spectrum.hpp"
#include "SurfaceType.hpp"
#include "Material.hpp"

namespace nagato
{
    class Emitter : public Material
    {
     public:

        Spectrum emittence;

        Emitter(SurfaceType t, Spectrum c, Spectrum e);

    };
}


#endif //PATHTRACING_EMITTER_HPP
