//
// Created by 長井亨 on 2018/09/05.
//

#ifndef PATHTRACING_BSDF_HPP
#define PATHTRACING_BSDF_HPP

#include <memory>
#include "../color/Spectrum.hpp"
#include "../core/Hit.hpp"

namespace nagato {
    class BSDF {
     public:
        explicit BSDF(Material *m);

        virtual Spectrum makeNewDirection(int *wavelengthIndex, Vector3 *newDirection, Ray &ray, Hit &surfaceInfo)= 0;

     protected:
        Material *material;
    };
}

#endif //PATHTRACING_BSDF_HPP
