//
// Created by kiki on 2018/07/18.
//

#include "Spectrum.hpp"
#include "Common.hpp"
#include "csv.h"
#include "Matrix3.hpp"

namespace nagato
{

    Spectrum::Spectrum()
    {
        spectrum.resize(static_cast<unsigned long>(resolution_ + 1), 0.0); // NOLINT
        samplePoints.resize(static_cast<unsigned long>(sample), 0);
    }

    void Spectrum::sample()
    {
//        std::cout << sample_ << " : " << resolution_ << std::endl;
        samplePoints = make_rand_array_unique(sample_, 0, resolution_);
    }

    Spectrum::Spectrum(double init_num)
    {
        spectrum.resize(static_cast<unsigned long>(resolution_ + 1), init_num); // NOLINT
        samplePoints.resize(static_cast<unsigned long>(sample), 0);
    }

    Spectrum::Spectrum() = default;

    Spectrum::Spectrum(std::string filename)
    {
        std::cout << "load spectrum data : " << filename << std::endl;
        io::CSVReader<2> in(filename);
        in.read_header(io::ignore_extra_column, "Wavelength", "Intensity");
        int wave;
        double intensity;
        std::vector<std::tuple<int, double>> spectrumData;

        // 波長データを取得
        while (in.read_row(wave, intensity)) {
//        std::cout << "wave, intensity : " << wave << ", " << intensity << std::endl;
            spectrumData.emplace_back(wave, intensity);
        }

        // データが一つの場合はすべての波長をその値に置き換える
        // データは380~780のカバーしていると仮定している
        if (spectrumData.size() == 1) {
            for (int i = 0; i < 401; i++) {
                spectrum.push_back(std::get<1>(spectrumData[0]));
            }
        } else if (spectrumData.size() > 1) {
            int diff = std::get<0>(spectrumData[1]) - std::get<0>(spectrumData[0]);
//        std::cout << "diff : " << diff << std::endl;

            // resolutionが1の場合は380~780すべてをカバーしていると仮定して
            // csvファイルからデータを読み取る
            if (diff == 1) {
                auto lastSpec = static_cast<int>(std::get<0>(spectrumData[0]) + (spectrumData.size() - 1) * diff);
//            std::cout << "lastSpec : " << lastSpec << std::endl;

                // 380nmのインデックスを調べる
                int index = 0;
                for (int i = 0; i < spectrumData.size(); ++i) {
                    if (std::get<0>(spectrumData[i]) == 380) {
                        index = i;
                        break;
                    }
                }

                for (int i = 0; i < 401; i++) {
                    spectrum.push_back(std::get<1>(spectrumData[i + index]));
                }

//            std::cout << "-------------" << std::endl;
//            std::cout << "spectrum data size : " << spectrum.size() << std::endl;
//            for (int i = 0; i < spectrum.size(); i++) {
//                std::cout << i + 380 << " : " << spectrum[i] << std::endl;
//            }
            } else if (diff == 5) {
                // 380nmのインデックスを調べる
                int index = 0;
                for (int i = 0; i < spectrumData.size(); ++i) {
                    if (std::get<0>(spectrumData[i]) == 380) {
                        index = i;
                        break;
                    }
                }

//            std::cout << "index : " << index << std::endl;
                for (int i = 0; i < 401; i++) {
                    double rate = (380 + i) % 5;
                    if (rate == 0) {
                        spectrum.push_back(std::get<1>(spectrumData[(i / 5) + index]));
//                    printf("%d : %f\n", 380 + i, std::get<1>(spectrumData[(i / 5) + index]));
                    } else {
                        int nowSpec = i / 5;
                        auto a = std::get<1>(spectrumData[(nowSpec) + index]);
                        auto b = std::get<1>(spectrumData[(nowSpec + 1) + index]);
                        double spec = (1.0 - rate / 5.0) * a + (rate / 5.0) * b;
//                    printf("%d : %f\t --- (%.5f, %.5f) \t --- rate : %3f\t --\n", 380 + i, spec, a, b, rate);
                        spectrum.push_back(spec);
                    }


                }

//            std::cout << "-------------" << std::endl;
//            std::cout << "spectrum data size : " << spectrum.size() << std::endl;
//            for (int i = 0; i < spectrum.size(); i++) {
//                printf("%d : %5f\n", i + 380, spectrum[i]);
//            }
            }
        } else {
            std::cerr << "スペクトルデータがありません : "
                      << filename << std::endl;
        }
    }

    ColorRGB Spectrum::toRGB() const
    {
        ColorRGB color;

        // スペクトルからXYZに変換する等色関数
        Spectrum red("../property/cie_1931_red.csv");
        Spectrum bule("../property/cie_1931_bule.csv");
        Spectrum green("../property/cie_1931_green.csv");

        auto spectrumX = red * *this;
        auto spectrumY = bule * *this;
        auto spectrumZ = green * *this;

        for (int i = 0; i < this->resolution_ + 1; ++i) {
            color.x += spectrumX.spectrum[i];
            color.y += spectrumY.spectrum[i];
            color.z += spectrumZ.spectrum[i];
        }

        double torgb[3][3] = {{2.3655,  -0.8971, -0.4683},
                              {-0.5151, 1.4264,  0.0887},
                              {0.0052,  -0.0144, 1.0089},};
        color.x = color.x * torgb[0][0] + color.y * torgb[0][1] + color.z * torgb[0][2];
        color.y = color.x * torgb[1][0] + color.y * torgb[1][1] + color.z * torgb[1][2];
        color.z = color.x * torgb[2][0] + color.y * torgb[2][1] + color.z * torgb[2][2];

        return color;

    }
}