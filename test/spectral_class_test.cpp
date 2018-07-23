//
// Created by 長井亨 on 2018/07/23.
//

#include "../src/Spectrum.hpp"

using namespace nagato;

double nt(int wavelength)
{
    return(sqrt(3.5139564 - 2.4812508E-2 * pow(wavelength, 2) +
                                   4.6252559E-2 * pow(wavelength, -2) +
                                   9.1313596E-3 * pow(wavelength, -4) -
                                   1.0777108E-3 * pow(wavelength, -6) +
                                   1.0819677E-4 * pow(wavelength, -8)));
}


int main()
{
    std::cout << "屈折率" << std::endl;
    for (int i = 380; i < 781; ++i) {
        std::cout << i << " : " << nt(i) << std::endl;
    }
    std::cout << "-----------------" << std::endl;

    Spectrum test(0.0);
    test.sample(10);

    printSample(test);

    std::cout << "-----------------" << std::endl;

    Random rng(100);
    int wavelength = test.samplePoints[rng.next(0, SAMPLE) % SAMPLE];
    printf("selected wavelength : %d\n", wavelength + 380);
    test.leaveOnePoint(wavelength);

    printSample(test);
}
