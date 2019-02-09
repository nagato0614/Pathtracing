//
// Created by 長井亨 on 2019-02-06.
//

#ifndef PATHTRACING_SIMPLESKY_HPP
#define PATHTRACING_SIMPLESKY_HPP

#include "Sky.hpp"

namespace nagato {
    class SimpleSky : public Sky {
     public :
        explicit SimpleSky(const Spectrum &s);

        Spectrum getRadiance(const Ray &ray) const override;

     private:
        Spectrum skyColor;
    };
}

#endif //PATHTRACING_SIMPLESKY_HPP
