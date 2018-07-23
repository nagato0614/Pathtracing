//
// Created by kiki on 2018/07/19.
//

#include "ColorRGB.hpp"

namespace nagato
{

    ColorRGB::ColorRGB(float color) : ColorRGB(color, color, color)
    {

    }

    ColorRGB::ColorRGB(float r, float g, float b) : r(r), b(b), g(g)
    {

    }

    void ColorRGB::spectrum2rgb(Spectrum s, Spectrum x, Spectrum y, Spectrum z)
    {
        const auto k = 1.0 / y.sum();
        const auto spectrumX = x * s;
        const auto spectrumY = y * s;
        const auto spectrumZ = z * s;

        ColorRGB XYZ(0);
        XYZ.r = static_cast<float>(spectrumX.sum() * k);
        XYZ.g = static_cast<float>(spectrumY.sum() * k);
        XYZ.b = static_cast<float>(spectrumZ.sum() * k);

        // XYZ色相系からRGBに変換する行列
        const float torgb[3][3] = {{3.2404542,  -1.5371385, -0.4985314},
                                    {-0.9692660, 1.8760108,  0.0415560},
                                    {0.0556434,  -0.2040259, 1.0572252},};
        r = XYZ.r * torgb[0][0] + XYZ.g * torgb[0][1] + XYZ.b * torgb[0][2];
        g = XYZ.r * torgb[1][0] + XYZ.g * torgb[1][1] + XYZ.b * torgb[1][2];
        b = XYZ.r * torgb[2][0] + XYZ.g * torgb[2][1] + XYZ.b * torgb[2][2];
    }

    void ColorRGB::normilize()
    {
        float sum = r + g + b;
        r /= sum;
        g /= sum;
        b /= sum;
    }
}
