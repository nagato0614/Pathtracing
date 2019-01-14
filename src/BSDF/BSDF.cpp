//
// Created by 長井亨 on 2018/09/06.
//

#include "BSDF.hpp"
#include "Lambert.hpp"
#include "Specular.hpp"
#include "Fresnel.hpp"

namespace nagato {


    float BSDF::f_r(Vector3 wi, Vector3 wo) {
        return 0;
    }

    float BSDF::pdf(Vector3 wi, Vector3 wo, Hit hitPoint) {
        return 0;
    }

    BSDF::BSDF(Spectrum color)
    : color(color){

    }

    BSDF *createBSDF(const Material &m) {
        BSDF *bsdf = nullptr;
        switch (m.type()) {
            case SurfaceType::Diffuse:
                bsdf = new Lambert(m.color);
                break;
            case SurfaceType::Mirror:
                bsdf = new Specular(m.color);
                break;
            case SurfaceType::Fresnel:
                bsdf = new Fresnel(m.color);
                break;
            case SurfaceType::Emitter:
                bsdf = new Lambert(m.color);
                break;
        }
        return bsdf;
    }
}