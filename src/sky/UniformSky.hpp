//
// Created by 長井亨 on 2019-02-04.
//

#ifndef PATHTRACING_UNIFORMSKY_HPP
#define PATHTRACING_UNIFORMSKY_HPP


#include "Sky.hpp"

namespace nagato {
    class UniformSky : public Sky {
     public:
        explicit UniformSky(const Spectrum &s);

        Spectrum getRadiance(const Ray &ray) const override;

     private:

        Spectrum skyColor;
    };
}


#endif //PATHTRACING_UNIFORMSKY_HPP
