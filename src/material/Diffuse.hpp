//
// Created by 長井亨 on 2019/01/29.
//

#ifndef PATHTRACING_DIFFUSE_HPP
#define PATHTRACING_DIFFUSE_HPP

#include "Material.hpp"

namespace nagato {
    class Diffuse : public Material {
     public:
        explicit Diffuse(Spectrum c);


     private:
    };
}

#endif //PATHTRACING_DIFFUSE_HPP
