//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_DIELECTRIC_HPP
#define PATHTRACING_DIELECTRIC_HPP


#include "Material.hpp"

namespace nagato
{
    class Dielectric : public Material
    {
     public:
        Dielectric(SurfaceType t, Spectrum c, Spectrum r);

     private:
        Spectrum refractance;
    };
}

#endif //PATHTRACING_DIELECTRIC_HPP
