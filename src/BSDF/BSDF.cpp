//
// Created by 長井亨 on 2018/09/06.
//

#include "BSDF.hpp"
#include <bits/shared_ptr.h>

namespace nagato {


    BSDF::BSDF(Material *m)
    : material(m)
    {

    }

    Material *BSDF::getMaterial()
    {
        return this->material;
    }
}