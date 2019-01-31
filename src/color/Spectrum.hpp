//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SPECTRUM_HPP
#define PATHTRACING_SPECTRUM_HPP

#include <vector>
#include <iostream>
#include "../core/Random.hpp"
#include "../core/Common.hpp"

namespace nagato {

    /**!
     * スペクトルを扱うデータ構造
     * 380nm ~ 780nmの可視光範囲のみ扱う前提
     */
    class Spectrum {
    public:


        // すべての波長をinit_numの値で初期化する
        explicit Spectrum(float init_num = 0.0);

        // 波長に対する反射率を保存したcsvから読み込む1
        explicit Spectrum(std::string filename);

        friend inline Spectrum operator*(Spectrum a, Spectrum b) {
            Spectrum spectrum(0.0);

            for (int i = 0; i < a.resolution_ + 1; ++i) {
                spectrum.spectrum[i] = a.spectrum[i] * b.spectrum[i];
            }
            return spectrum;

        }

        friend inline Spectrum operator+(Spectrum a, Spectrum b) {
            Spectrum spectrum(0.0);

            for (int i = 0; i < a.resolution_ + 1; ++i) {
                spectrum.spectrum[i] = a.spectrum[i] + b.spectrum[i];
            }
            return spectrum;

        }

        friend inline Spectrum operator-(Spectrum a, Spectrum b) {
            Spectrum spectrum(0.0);

            for (int i = 0; i < a.resolution_ + 1; ++i) {
                spectrum.spectrum[i] = a.spectrum[i] - b.spectrum[i];
            }
            return spectrum;

        }

        friend inline Spectrum operator/(Spectrum a, Spectrum b) {
            Spectrum spectrum(0.0);

            for (int i = 0; i < a.resolution_ + 1; ++i) {
                spectrum.spectrum[i] = a.spectrum[i] / b.spectrum[i];
            }
            return spectrum;

        }

        friend inline Spectrum operator/(Spectrum a, float b) {
            Spectrum spectrum(0.0);

            for (int i = 0; i < a.resolution_ + 1; ++i) {
                spectrum.spectrum[i] = a.spectrum[i] / b;
            }

            return spectrum;
        }

        friend inline Spectrum operator*(Spectrum a, float b) {
            Spectrum spectrum(0.0);

            for (int i = 0; i < a.resolution_ + 1; ++i) {
                spectrum.spectrum[i] = a.spectrum[i] * b;
            }

            return spectrum;
        }

        // サンプル点ランダムに一つ抽出
        inline int getOneSampledPoint() {
            return Random::Instance().nextInt(0, int(sample_));
        }

        // すべての波長要素を足し合わせる
        float sum() const;

        // 波長の最大値を見つけて返す
        float findMaxSpectrum();

        // 寄与を更新する
        void addContribution(Spectrum weight, Spectrum emitter);

        // [0,1]の範囲に正規化する
        void normilize();

        // あるポイントだけを残して他の値を0にする
        void leaveOnePoint(int index);

        size_t sample_ = SAMPLE;
        int resolution_ = RESOLUTION;

        // 実際の波長を保存する
        float spectrum[RESOLUTION];
    };

    void printSpectrum(Spectrum s);

    void printSample(Spectrum s);
}

#endif //PATHTRACING_SPECTRUM_HPP
