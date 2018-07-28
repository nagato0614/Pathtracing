//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_MATERIAL_HPP
#define PATHTRACING_MATERIAL_HPP

#include "SurfaceType.hpp"
#include "Spectrum.hpp"

namespace nagato
{
    class Material
    {
     public:
        Spectrum color;
        Spectrum emitter;

        Material(SurfaceType t, Spectrum c, Spectrum e);

        SurfaceType type();

     private:
        SurfaceType surfaceType;
    };
}
#endif //PATHTRACING_MATERIAL_HPP
