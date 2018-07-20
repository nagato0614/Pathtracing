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
        explicit ColorRGB(double color = 0.0);

        ColorRGB(double r, double g, double b);

        void spectrum2rgb(Spectrum s, Spectrum red, Spectrum blue, Spectrum green);

        double r;
        double g;
        double b;
    };
};


#endif //PATHTRACING_COLORRGB_HPP
