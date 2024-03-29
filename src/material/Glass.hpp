//
// Created by 長井亨 on 2019/01/29.
//

#ifndef PATHTRACING_GLASS_HPP
#define PATHTRACING_GLASS_HPP


#include "Material.hpp"
#include "../BSDF/Fresnel.hpp"


namespace nagato {
    class Glass : public Material {
     public:
        explicit Glass(Spectrum c, float ior);
     private:
        float ior;
    };
}

#endif //PATHTRACING_GLASS_HPP
