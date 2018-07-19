//
// Created by kiki on 2018/07/19.
//

#ifndef PATHTRACING_COLORRGB_HPP
#define PATHTRACING_COLORRGB_HPP

#include "Vector3.hpp"

namespace nagato
{
    class ColorRGB : public Vector3
    {
     public:
        ColorRGB();

        ColorRGB(double r, double g, double b);
    };
}


#endif //PATHTRACING_COLORRGB_HPP
