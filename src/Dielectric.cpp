//
// Created by 長井亨 on 2018/07/23.
//

#include "Dielectric.hpp"
namespace nagato {

    Dielectric::Dielectric(SurfaceType t, Spectrum c, Spectrum r) : Material(t, c), refractance(r)
    {

    }
}
