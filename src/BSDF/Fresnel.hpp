//
// Created by 長井亨 on 2018/09/06.
//

#ifndef PATHTRACING_FRESNEL_HPP
#define PATHTRACING_FRESNEL_HPP

#include "BSDF.hpp"

namespace nagato {
    class Fresnel : public BSDF {
     public:
        explicit Fresnel(Spectrum c);

        Fresnel(Spectrum c, float ior);

        Spectrum makeNewDirection(int *wavelengthIndex,
                                  Vector3 *newDirection,
                                  Ray &ray,
                                  const Hit &surfaceInfo,
                                  float *pdf) const override;

     private:
        float ior;
    };

    std::shared_ptr<Fresnel> createFresnel(Spectrum c, float ior);
}

#endif //PATHTRACING_FRESNEL_HPP
