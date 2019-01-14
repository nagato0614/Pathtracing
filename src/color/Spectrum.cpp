//
// Created by kiki on 2018/07/18.
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
        int wave;
        float intensity;
        std::vector<std::tuple<int, float>> spectrumData;
        std::vector<float> s;

        // 波長データを取得
        while (in.read_row(wave, intensity)) {
            spectrumData.emplace_back(wave, intensity);
        }

        // データが一つの場合はすべての波長をその値に置き換える
        // データは380~780のカバーしていると仮定している
        if (spectrumData.size() == 1) {
            for (int i = 0; i < 401; i++) {
                s.push_back(std::get<1>(spectrumData[0]));
            }
        } else if (spectrumData.size() > 1) {
            int diff = std::get<0>(spectrumData[1]) - std::get<0>(spectrumData[0]);

            // resolutionが1の場合は380~780すべてをカバーしていると仮定して
            // csvファイルからデータを読み取る
            if (diff == 1) {

                // 380nmのインデックスを調べる
                int index = 0;
                for (int i = 0; i < spectrumData.size(); ++i) {
                    if (std::get<0>(spectrumData[i]) == 380) {
                        index = i;
                        break;
                    }
                }

                for (int i = 0; i < 401; i++) {
                    s.push_back(std::get<1>(spectrumData[i + index]));
                }

            } else if (diff == 5) {
                // 380nmのインデックスを調べる
                int index = 0;
                for (int i = 0; i < spectrumData.size(); ++i) {
                    if (std::get<0>(spectrumData[i]) == 380) {
                        index = i;
                        break;
                    }
                }

                for (int i = 0; i < 401; i++) {
                    float rate = (380 + i) % 5;
                    if (rate == 0) {
                    } else {
                        int nowSpec = i / 5;
                        auto a = std::get<1>(spectrumData[(nowSpec) + index]);
                        auto b = std::get<1>(spectrumData[(nowSpec + 1) + index]);
                        float spec = (1.0 - rate / 5.0) * a + (rate / 5.0) * b;
                        s.push_back(spec);
                    }


                }

            }

            assert((400 % RESOLUTION) == 0);
            for (int i = 0; i < RESOLUTION; i++) {
                spectrum[i] = s[(400 / RESOLUTION) * i];
            }
        } else {
            std::cerr << "スペクトルデータがありません : "
                      << filename << std::endl;
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
        for (int i = 0; i < resolution_ + 1; ++i) {
            if (index != i)
                spectrum[i] = 0.0;
            else
                spectrum[i] *= RESOLUTION;
        }
    }

    void printSpectrum(Spectrum s) {
        for (int i = 0; i < 401; i++) {
            printf("%3d \t : %10.5f\n", 380 + i, s.spectrum[i]);
        }
    }

}
