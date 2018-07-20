//
// Created by kiki on 2018/07/19.
//

#include "ColorRGB.hpp"

namespace nagato
{

    ColorRGB::ColorRGB(double color) : ColorRGB(color, color, color)
    {

    }

    ColorRGB::ColorRGB(double r, double g, double b) : r(r), b(b), g(g)
    {

    }

    void ColorRGB::spectrum2rgb(Spectrum s, Spectrum red, Spectrum green, Spectrum blue)
    {
        const auto spectrumX = red * s;
        const auto spectrumY = green * s;
        const auto spectrumZ = blue * s;

        ColorRGB buff(0);
        for (int i = 0; i < s.resolution_ + 1; ++i) {
            buff.r += spectrumX.spectrum[i];
            buff.g += spectrumY.spectrum[i];
            buff.b += spectrumZ.spectrum[i];
        }

        double torgb[3][3] = {{3.2404542, -1.5371385, -0.4985314},
                              {-0.9692660, 1.8760108, 0.0415560},
                              {0.0556434, -0.2040259, 1.0572252},};
        r = buff.r * torgb[0][0] + buff.g * torgb[0][1] + buff.b * torgb[0][2];
        g = buff.r * torgb[1][0] + buff.g * torgb[1][1] + buff.b * torgb[1][2];
        b = buff.r * torgb[2][0] + buff.g * torgb[2][1] + buff.b * torgb[2][2];
    }
}
