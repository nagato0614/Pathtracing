//
// Created by 長井亨 on 2018/07/19.
//

#include "ColorRGB.hpp"

namespace nagato
{

    ColorRGB::ColorRGB(float color) : ColorRGB(color, color, color)
    {

    }

    ColorRGB::ColorRGB(float r, float g, float b) : r(r), g(g), b(b)
    {

    }

    void ColorRGB::spectrum2rgb(Spectrum s, Spectrum xbar, Spectrum ybar, Spectrum zbar)
    {

        const auto k = 1.0 / ybar.sum();
        const auto spectrumX = xbar * s;
        const auto spectrumY = ybar * s;
        const auto spectrumZ = zbar * s;

        ColorRGB XYZ(0);
        XYZ.r = static_cast<float>(spectrumX.sum() * k);
        XYZ.g = static_cast<float>(spectrumY.sum() * k);
        XYZ.b = static_cast<float>(spectrumZ.sum() * k);

        if (XYZ.sum() == 0 || XYZ.g == 0.0) {
            r = 0.0;
            g = 0.0;
            b = 0.0;
            return;
        }
        float Y = XYZ.g;
        float x = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);
        float y = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);

        Y = nagato::clamp(0.0, 1.0, static_cast<const float>(Y / (1.0 + Y)));

        XYZ.r = (Y / y) * x;
        XYZ.g = Y;
        XYZ.b = static_cast<float>((Y / y) * (1.0 - x - y));

        // XYZ色相系からRGBに変換する行列
        const float torgb[3][3] = {{3.2404542,  -1.5371385, -0.4985314},
                                   {-0.9692660, 1.8760108,  0.0415560},
                                   {0.0556434,  -0.2040259, 1.0572252},};

        ColorRGB buff;
        buff.r = XYZ.r * torgb[0][0] + XYZ.g * torgb[0][1] + XYZ.b * torgb[0][2];
        buff.g = XYZ.r * torgb[1][0] + XYZ.g * torgb[1][1] + XYZ.b * torgb[1][2];
        buff.b = XYZ.r * torgb[2][0] + XYZ.g * torgb[2][1] + XYZ.b * torgb[2][2];

        if (1.0 < buff.max()) {
            buff.scale();
        }

        buff.clamp();
        buff.gammaCorrection(static_cast<float>(1.0 / 2.2));

        r = buff.r;
        g = buff.g;
        b = buff.b;
    }

    void ColorRGB::normilize()
    {
        float sum = r + g + b;
        if (sum == 0.0) {
            return;
        }
        r /= sum;
        g /= sum;
        b /= sum;
    }

    float ColorRGB::max()
    {
        return std::max(r, std::max(g, b));
    }

    void ColorRGB::scale()
    {
        auto scale_facter = static_cast<const float>(1.0 / max());
        r *= scale_facter;
        g *= scale_facter;
        b *= scale_facter;
    }

    void ColorRGB::clamp()
    {
        r = nagato::clamp(0.0, 1.0, r);
        g = nagato::clamp(0.0, 1.0, g);
        b = nagato::clamp(0.0, 1.0, b);
    }

    void ColorRGB::gammaCorrection(float inverse_gamma)
    {
        r = std::pow(r, inverse_gamma);
        g = std::pow(g, inverse_gamma);
        b = std::pow(b, inverse_gamma);
    }

    int ColorRGB::r255() const
    {
        return static_cast<int>(std::floor(r * 255.0));
    }

    int ColorRGB::g255() const
    {
        return static_cast<int>(std::floor(g * 255.0));
    }

    int ColorRGB::b255() const
    {
        return static_cast<int>(std::floor(b * 255.0));
    }

    float ColorRGB::sum()
    {
        return r + g + b;
    }

}
