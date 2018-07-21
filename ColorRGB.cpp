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
        auto k = 1.0 / green.sum();
        const auto spectrumX = red * s;
        const auto spectrumY = green * s;
        const auto spectrumZ = blue * s;

        ColorRGB XYZ(0);
        XYZ.r = spectrumX.sum() * k;
        XYZ.g = spectrumY.sum() * k;
        XYZ.b = spectrumZ.sum() * k;

        // XYZ色相系からRGBに変換する行列
        const double torgb[3][3] = {{3.2404542,  -1.5371385, -0.4985314},
                                    {-0.9692660, 1.8760108,  0.0415560},
                                    {0.0556434,  -0.2040259, 1.0572252},};
        r = XYZ.r * torgb[0][0] + XYZ.g * torgb[0][1] + XYZ.b * torgb[0][2];
        g = XYZ.r * torgb[1][0] + XYZ.g * torgb[1][1] + XYZ.b * torgb[1][2];
        b = XYZ.r * torgb[2][0] + XYZ.g * torgb[2][1] + XYZ.b * torgb[2][2];
    }
}
