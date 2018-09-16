//
// Created by 長井亨 on 2018/09/05.
//

#include "DiffuseLight.hpp"

namespace nagato {
    DiffuseLight::DiffuseLight(Spectrum I, LightType type, Object *o)
    : Light(I, type), object(o)
    {

    }

    Spectrum DiffuseLight::samplePoint(Hit hit, Vector3 *sampledPoint, float *pdf)
    {
        return Spectrum();
    }
}
