//
// Created by kiki on 2018/07/23.
//


#include <fstream>
#include "../src/Spectrum.hpp"
#include "../src/ColorRGB.hpp"

using namespace nagato;

int main()
{

    // スペクトルからXYZに変換する等色関数
    const Spectrum xbar("../property/cie_sco_2degree_xbar.csv");
    const Spectrum ybar("../property/cie_sco_2degree_ybar.csv");
    const Spectrum zbar("../property/cie_sco_2degree_zbar.csv");

    Spectrum light("../property/cie_si_d65.csv");
    Spectrum spectrum(1.0);

    const Spectrum color1("../property/macbeth_19_white.csv");
    const Spectrum color2("../property/macbeth_15_red.csv");
    const Spectrum color3("../property/macbeth_13_blue.csv");


    auto weight = color2;
    printSpectrum(weight);
    spectrum = light * weight;

//    printSpectrum(spectrum);

    const auto k = 1.0 / (ybar).sum();

    const auto spectrumX = xbar * spectrum;
    const auto spectrumY = ybar * spectrum;
    const auto spectrumZ = zbar * spectrum;

    ColorRGB XYZ(0);
    XYZ.r = static_cast<float>(spectrumX.sum() * k);
    XYZ.g = static_cast<float>(spectrumY.sum() * k);
    XYZ.b = static_cast<float>(spectrumZ.sum() * k);

    float Y = XYZ.g;
    float x = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);
    float y = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);

    Y = clamp(0.0, 1.0, static_cast<const float>(Y / (1.0 + Y)));

    XYZ.r = (Y / y) * x;
    XYZ.g = Y;
    XYZ.b = static_cast<float>((Y / y) * (1.0 - x - y));
    printf("x : %f\n", XYZ.r);
    printf("y : %f\n", XYZ.g);
    printf("z : %f\n", XYZ.b);


    // XYZ色相系からRGBに変換する行列
    const float torgb[3][3] = {{3.2404542,  -1.5371385, -0.4985314},
                               {-0.9692660, 1.8760108,  0.0415560},
                               {0.0556434,  -0.2040259, 1.0572252},};

    ColorRGB result;
    result.r = XYZ.r * torgb[0][0] + XYZ.g * torgb[0][1] + XYZ.b * torgb[0][2];
    result.g = XYZ.r * torgb[1][0] + XYZ.g * torgb[1][1] + XYZ.b * torgb[1][2];
    result.b = XYZ.r * torgb[2][0] + XYZ.g * torgb[2][1] + XYZ.b * torgb[2][2];

    if (1.0 < result.max()) {
        result.scale();
    }
    result.clamp();
    result.gammaCorrection(static_cast<float>(1.0 / 2.2));

    printf("r : %f\n", result.r);
    printf("g : %f\n", result.g);
    printf("b : %f\n", result.b);
    printf("r[0, 255] : %d\n", int(result.r));
    printf("g[0, 255] : %d\n", int(result.g));
    printf("b[0, 255] : %d\n", int(result.b));

    const int width = 480;
    const int height = 360;

    std::vector<Spectrum> S(width * height);

    for (auto &i : S) {
        i = spectrum;
    }

    writePPM("spectrum_test.ppm", S, width, height, xbar, ybar, zbar);
}
