//
// Created by 長井亨 on 2019/01/29.
//

#ifndef PATHTRACING_DIFFUSELIGHT_HPP
#define PATHTRACING_DIFFUSELIGHT_HPP


#include "Material.hpp"

namespace nagato {
    class DiffuseLight : public Material {
     public:
        explicit DiffuseLight(Spectrum e, float l);
     private:
    };
}

#endif //PATHTRACING_DIFFUSELIGHT_HPP
