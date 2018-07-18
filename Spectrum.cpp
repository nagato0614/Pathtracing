//
// Created by kiki on 2018/07/18.
//

#include "Spectrum.hpp"
#include "Common.hpp"

namespace nagato {

    Spectrum::Spectrum(int resolution, int sample) : resolution_(resolution), sample_(sample)
    {
        spectrum.resize(static_cast<unsigned long>(resolution), 0.0);
        samplePoints.resize(static_cast<unsigned long>(sample), 0);
    }

    void Spectrum::sample()
    {
        samplePoints = make_rand_array_unique(static_cast<size_t>(sample_), 0, resolution_);
    }
}