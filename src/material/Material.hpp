//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_MATERIAL_HPP
#define PATHTRACING_MATERIAL_HPP

#include "../core/SurfaceType.hpp"
#include "../color/Spectrum.hpp"
#include "../BSDF/BSDF.hpp"

namespace nagato
{
    class BSDF;
    class Material
    {
     public:
        Spectrum color;
        Spectrum emitter;

        Material(SurfaceType t, Spectrum c, Spectrum e = Spectrum(), float emitterL = 1.0);

        ~Material();

        BSDF &getBSDF();

        SurfaceType type();

        std::string typeName();

        const Spectrum &getRefraction() const;

        void setRefraction(const Spectrum &refraction);

     private :

        BSDF *bsdf = nullptr;
        Spectrum refraction;
        SurfaceType surfaceType;
    };
}
#endif //PATHTRACING_MATERIAL_HPP
