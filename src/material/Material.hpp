//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_MATERIAL_HPP
#define PATHTRACING_MATERIAL_HPP

#include "../core/SurfaceType.hpp"
#include "../color/Spectrum.hpp"
#include "../BSDF/BSDF.hpp"

namespace nagato {
    class BSDF;

    class Material {
    public:


        Material(SurfaceType t, Spectrum c, Spectrum e = Spectrum(), float emitterL = 1.0);

        ~Material();

        BSDF &getBSDF() const ;

        SurfaceType type() const;

        std::string typeName();

        const Spectrum &getColor() const;

        const Spectrum &getEmitter() const;

    protected :
        Spectrum color;
        Spectrum emitter;
        std::shared_ptr<BSDF> bsdf;
        SurfaceType surfaceType;
    };
}
#endif //PATHTRACING_MATERIAL_HPP
