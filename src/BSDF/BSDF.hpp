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

    /**
     * BSDFを扱う大本となるクラス
     */
    class BSDF
    {
     public:
        explicit BSDF(Material *m);

        /**
         * 現在のrayから反射方向とweightを計算する関数
         * @param wavelengthIndex
         * @param newDirection
         * @param ray
         * @param surfaceInfo
         * @return materialが持つ反射率
         */
        virtual Spectrum makeNewDirection(
                int *wavelengthIndex,
                Vector3 *newDirection,
                Ray &ray,
                const Hit &surfaceInfo) const = 0;

        Material *getMaterial();

     protected:
        Material *material;
    };

    BSDF *createBSDF(Material *material);
}

#endif //PATHTRACING_BSDF_HPP
