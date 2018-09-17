//
// Created by 長井亨 on 2018/09/05.
//

#ifndef PATHTRACING_BSDF_HPP
#define PATHTRACING_BSDF_HPP

#include <memory>
#include "../color/Spectrum.hpp"
#include "../core/Hit.hpp"
#include "../material/Material.hpp"

namespace nagato
{
    class Material;

    class Hit;

    class BSDF
    {
     public:
        explicit BSDF(Material *m);

        virtual Spectrum makeNewDirection(int *wavelengthIndex,
                                          Vector3 *newDirection,
                                          Ray &ray,
                                          Hit &surfaceInfo) const = 0;

        Material *getMaterial();

     protected:
        Material *material;
    };

    BSDF *createBSDF(Material *material);
}

#endif //PATHTRACING_BSDF_HPP
