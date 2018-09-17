//
// Created by 長井亨 on 2018/09/06.
//

#include "BSDF.hpp"
#include "Lambert.hpp"
#include "Specular.hpp"
#include "Fresnel.hpp"
#include <bits/shared_ptr.h>

namespace nagato
{


    BSDF::BSDF(Material *m)
            : material(m)
    {

    }

    Material *BSDF::getMaterial()
    {
        return this->material;
    }

    BSDF *createBSDF(Material *material)
    {
        BSDF *bsdf = nullptr;
        switch (material->type()) {
            case SurfaceType::Diffuse:
                bsdf = new Lambert(material);
                break;
            case SurfaceType::Mirror:
                bsdf = new Specular(material);
                break;
            case SurfaceType::Fresnel:
                bsdf = new Fresnel(material);
                break;
            case SurfaceType::Emitter:
                break;
        }
        return bsdf;
    }
}