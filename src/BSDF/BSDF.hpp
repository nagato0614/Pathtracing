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
        BSDF(Spectrum color);

        /**
         * 現在のrayから反射方向とweightを計算する関数
         * @param wavelengthIndex
         * @param newDirection
         * @param ray
         * @param surfaceInfo
         * @return materialが持つ反射率
         */
        virtual Spectrum makeNewDirection(int *wavelengthIndex, Vector3 *newDirection, Ray &ray, const Hit &surfaceInfo, float *pdf) const = 0;


        // 反射率を返す
        virtual Spectrum f_r(const Vector3 &wi, const Vector3 &wo) const ;

        // wi方向に反射する確率
        virtual float pdf(const Vector3 &wi, const Vector3 &wo, const Hit &hitPoint) const ;

        virtual float f(const Vector3 &wi, const Vector3 &wo) const ;

     protected:
        Spectrum color;
    };

    BSDF *createBSDF(const Material &material);
}

#endif //PATHTRACING_BSDF_HPP
