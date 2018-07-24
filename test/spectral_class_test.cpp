//
// Created by 長井亨 on 2018/07/23.
//

#include "../src/Spectrum.hpp"

using namespace nagato;

int main()
{
    Spectrum sampledSpectrum(0.0);
    sampledSpectrum.sample(1999);

    std::cout << "屈折率" << std::endl;
    Spectrum refraction("../property/SiO2.csv");
//    printSpectrum(refraction);

    Random rng(time(NULL));
    std::cout << "random value : " << rng.next(0, static_cast<int>(sampledSpectrum.samplePoints.size())) << std::endl;

    int wavelength = 0;

    printSpectrum(sampledSpectrum);

    // サンプル点を１つにしてそれ以外の影響を0にする
    if (sampledSpectrum.samplePoints.size() > 1) {
        wavelength = sampledSpectrum.samplePoints[rng.next(0, SAMPLE - 1)];
        sampledSpectrum.leaveOnePoint(wavelength);
    }

    std::cout << "wavelength : " << wavelength + 380 << std::endl;
    std::cout << "sample point number : " << sampledSpectrum.samplePoints.size() << std::endl;

    printSpectrum(sampledSpectrum);
    const float ior = refraction.spectrum[wavelength];

    std::cout << "refraction : " << ior << std::endl;
}
