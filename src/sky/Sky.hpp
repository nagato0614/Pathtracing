//
// Created by 長井亨 on 2019-02-04.
//

#ifndef PATHTRACING_SKY_HPP
#define PATHTRACING_SKY_HPP

#include "../color/Spectrum.hpp"
#include "../core/Ray.hpp"

namespace nagato {
    class Sky {
     public:
        virtual Spectrum getRadiance(const Ray &ray) const = 0;
    };
}

#endif //PATHTRACING_SKY_HPP
