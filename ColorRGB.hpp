//
// Created by kiki on 2018/07/19.
//

#ifndef PATHTRACING_COLORRGB_HPP
#define PATHTRACING_COLORRGB_HPP

#include "Vector3.hpp"
#include "Spectrum.hpp"

namespace nagato
{
    class Spectrum;
    class ColorRGB
    {
     public:
        explicit ColorRGB(float color = 0.0);

        ColorRGB(float r, float g, float b);

        void spectrum2rgb(Spectrum s, Spectrum x, Spectrum y, Spectrum z);

        void normilize();

        float r;
        float g;
        float b;
    };
};


#endif //PATHTRACING_COLORRGB_HPP
