//
// Created by 長井亨 on 2018/07/18.
//

#include "Spectrum.hpp"
#include "../core/csv.h"

namespace nagato {


    Spectrum::Spectrum(float init_num) {
        for (auto &s : spectrum) {
            s = init_num;
        }
    }

    Spectrum::Spectrum(std::string filename) {
        io::CSVReader<2> in(filename);
        in.read_header(io::ignore_extra_column, "Wavelength", "Intensity");
        float wave;
        float intensity;
        std::vector<float> lambda;
        std::vector<float> v;
        int n = 0;

        // 波長データを取得
        while (in.read_row(wave, intensity)) {
            lambda.push_back(wave);
            v.push_back(intensity);
            n++;
        }

        for (int i = 0; i < nSamples; i++) {
            auto start = lerp(float(i) / nSamples,
                              minSpectral, maxSpectral);
            auto end = lerp(float(i + 1) / nSamples,
                            minSpectral, maxSpectral);
            spectrum[i] = averageSpectrumSamples(lambda, v, n, start, end);
        }
    }

    float Spectrum::findMaxSpectrum() {
        float max = 0.0;
        for (auto i : spectrum) {
            if (i > max)
                max = i;
        }
        return max;
    }

    void Spectrum::addContribution(Spectrum weight, Spectrum emitter) {

    }

    float Spectrum::sum() const {
        float sum = 0.0;
        for (auto i : spectrum) {
            sum += i;
        }
        return sum;
    }

    void Spectrum::normilize() {
        float sum = 0.0;
        for (auto i : spectrum) {
            sum += i;
        }

        for (auto &i : spectrum)
            i /= sum;
    }

    void Spectrum::leaveOnePoint(int index) {
        for (int i = 0; i < resolution_; ++i) {
            if (index != i)
                spectrum[i] = 0.0;
            else
                spectrum[i] *= nSamples;
        }
    }

    Spectrum Spectrum::operator+=(const Spectrum &s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] += s.spectrum[i];
        }

        return spectrum;
    }

    Spectrum Spectrum::operator+=(float s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] += s;
        }

        return spectrum;
    }

    Spectrum Spectrum::operator-=(const Spectrum &s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] -= s.spectrum[i];
        }

        return spectrum;
    }

    Spectrum Spectrum::operator-=(float s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] -= s;
        }

        return spectrum;
    }

    Spectrum Spectrum::operator*=(const Spectrum &s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] *= s.spectrum[i];
        }

        return spectrum;
    }

    Spectrum Spectrum::operator*=(float s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] *= s;
        }

        return spectrum;
    }

    Spectrum Spectrum::operator/=(const Spectrum &s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] /= s.spectrum[i];
        }

        return spectrum;
    }

    Spectrum Spectrum::operator/=(float s) {
        Spectrum spectrum(0.0);
        for (int i = 0; i < resolution_; i++) {
            spectrum.spectrum[i] /= s;
        }

        return spectrum;
    }

    const float Spectrum::operator[](int i) const {
        if (i < 0 || nSamples <= i) {
            fprintf(stderr, "out of bounce index\n");
            exit(1);
        }
        return spectrum[i];
    }

    float &Spectrum::operator[](int i) {
        return spectrum[i];
    }

    void printSpectrum(Spectrum s) {
        for (int i = 0; i < nSamples; i++) {
            printf("%3f \t : %10.5f\n", 380 + i * resolution, s.spectrum[i]);
        }
    }

    Spectrum nagato::operator*(const Spectrum &a, const Spectrum &b) {
        Spectrum spectrum(0.0);

        for (int i = 0; i < a.resolution_; ++i) {
            spectrum.spectrum[i] = a.spectrum[i] * b.spectrum[i];
        }
        return spectrum;

    }

    Spectrum nagato::operator+(const Spectrum &a, const Spectrum &b) {
        Spectrum spectrum(0.0);

        for (int i = 0; i < a.resolution_; ++i) {
            spectrum.spectrum[i] = a.spectrum[i] + b.spectrum[i];
        }
        return spectrum;

    }

    Spectrum nagato::operator-(const Spectrum &a, const Spectrum &b) {
        Spectrum spectrum(0.0);

        for (int i = 0; i < a.resolution_; ++i) {
            spectrum.spectrum[i] = a.spectrum[i] - b.spectrum[i];
        }
        return spectrum;

    }

    Spectrum nagato::operator/(const Spectrum &a, const Spectrum &b) {
        Spectrum spectrum(0.0);

        for (int i = 0; i < a.resolution_; ++i) {
            spectrum.spectrum[i] = a.spectrum[i] / b.spectrum[i];
        }
        return spectrum;

    }

    Spectrum nagato::operator/(const Spectrum &a, float b) {
        Spectrum spectrum(0.0);

        for (int i = 0; i < a.resolution_; ++i) {
            spectrum.spectrum[i] = a.spectrum[i] / b;
        }

        return spectrum;
    }

    Spectrum nagato::operator*(const Spectrum &a, float b) {
        Spectrum spectrum(0.0);

        for (int i = 0; i < a.resolution_; ++i) {
            spectrum.spectrum[i] = a.spectrum[i] * b;
        }

        return spectrum;
    }

    Spectrum nagato::operator*(float a, const Spectrum &b) {
        Spectrum s(0.0);

        for (int i = 0; i < b.resolution_; i++) {
            s.spectrum[i] = a * b.spectrum[i];
        }

        return s;
    }

    Spectrum makeSpectrum(const std::vector<float> &lambda,
                          const std::vector<float> &v,
                          int size) {

        Spectrum spectrum(0.0f);

        for (int i = 0; i < nSamples; i++) {
            auto start = lerp(float(i) / nSamples,
                              minSpectral, maxSpectral);
            auto end = lerp(float(i + 1) / nSamples,
                            minSpectral, maxSpectral);
            spectrum[i] = averageSpectrumSamples(lambda, v, size, start, end);
        }
        return spectrum;
    }

    float averageSpectrumSamples(const std::vector<float> &lambda,
                                 const std::vector<float> &v,
                                 int n,
                                 float lambdaStart,
                                 float lambdaEnd) {

        // Handle cases with out-of-bounds range or single sample only

        // サンプル点がspdの最短波長よりも短い場合はspdの最短波長の寄与を返す
        if (lambdaEnd <= lambda[0])
            return v[0];

        // サンプル点がspdの最長波長よりも長い場合はspdの最長波長の寄与を返す.
        if (lambdaStart >= lambda[n - 1])
            return v[n - 1];

        // サンプル数が１つだけの場合
        if (n == 1)
            return v[0];

        float sum = 0;
        // Add contributions of constant segments before/after samples

        // lambdaStartがspdの最短波長よりも短い場合
        if (lambdaStart < lambda[0])
            sum += v[0] * (lambda[0] - lambdaStart);

        // lambdaEndがspdの最長波長よりも長い場合
        if (lambdaEnd > lambda[n - 1])
            sum += v[n - 1] * (lambdaEnd - lambda[n - 1]);

        // Advance to first relevant wavelength segment

        // lambdaStartよりも短く近い波長を探す
        int i = 0;
        while (lambdaStart > lambda[i + 1])
            i++;

        // Loop over wavelength sample segments and add contributions
        // lambdaStartからlambdaStart内にある波長の寄与を取得し平均化する

        auto interp = [lambda, v](float w, int i) {
            return lerp((w - lambda[i]) / (lambda[i + 1] - lambda[i]),
                        v[i], v[i + 1]);
        };

        for (; i + 1 < n && lambdaEnd >= lambda[i]; i++) {
            auto segLambdaStart = std::max(lambdaStart, lambda[i]);
            auto segLambdaEnd = std::min(lambdaEnd, lambda[i + 1]);

            sum += 0.5 * (interp(segLambdaStart, i) + interp(segLambdaEnd, i))
                   * (segLambdaEnd - segLambdaStart);
        }

        return sum / (lambdaEnd - lambdaStart);
    }

    Spectrum loadSPDFile(std::string filename) {
        io::CSVReader<2> in(filename);
        in.read_header(io::ignore_extra_column, "Wavelength", "Intensity");
        float wave;
        float intensity;
        std::vector<float> lambda;
        std::vector<float> v;
        int n = 0;

        while (in.read_row(wave, intensity)) {
            n++;
            lambda.push_back(wave);
            v.push_back(intensity);
        }

        return makeSpectrum(lambda, v, n);
    }

}
