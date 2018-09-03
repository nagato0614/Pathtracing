//
// Created by 長井亨 on 2018/09/03.
//

#ifndef PATHTRACING_LIGHT_HPP
#define PATHTRACING_LIGHT_HPP


#include "../color/Spectrum.hpp"
#include "../core/Hit.hpp"

namespace nagato
{
    enum LightType {
        Point,
        SpotLight,
        DiffuseLight
    };
    class Light
    {
     public:
        LightType type() { return lightType; }
        virtual Spectrum SmplePoint(Hit hit, Vector3 *sampledPoint);

     private:
        Spectrum I;
        LightType lightType;
    };
}

#endif //PATHTRACING_LIGHT_HPP
