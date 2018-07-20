//
// Created by kiki on 2018/07/18.
//

#ifndef PATHTRACING_SPECTRUM_HPP
#define PATHTRACING_SPECTRUM_HPP

#include <vector>
#include <iostream>
#include "Random.hpp"
#include "ColorRGB.hpp"
#include "Common.hpp"

namespace nagato
{

    /**!
     * スペクトルを扱うデータ構造
     * 380nm ~ 780nmの可視光範囲のみ扱う前提
     */
    class Spectrum
    {
     public:

        Spectrum();

        // すべての波長をinit_numの値で初期化する
        explicit Spectrum(double init_num);

        // 波長に対する反射率を保存したcsvから読み込む1
        explicit Spectrum(std::string filenam);

        // 設定したサンプル数だけランダムにサンプル点を決める
        void sample();

        friend inline Spectrum operator*(Spectrum a, Spectrum b)
        {
            Spectrum s;

            for (int i = 0; i < a.resolution_; ++i) {
                s.spectrum[i] = a.spectrum[i] * b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator+(Spectrum a, Spectrum b)
        {
            Spectrum s;

            for (int i = 0; i < a.resolution_; ++i) {
                s.spectrum[i] = a.spectrum[i] + b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator-(Spectrum a, Spectrum b)
        {
            Spectrum s;

            for (int i = 0; i < a.resolution_; ++i) {
                s.spectrum[i] = a.spectrum[i] - b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator/(Spectrum a, Spectrum b)
        {
            Spectrum s;

            for (int i = 0; i < a.resolution_; ++i) {
                s.spectrum[i] = a.spectrum[i] / b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator/(Spectrum a, double b)
        {
            Spectrum s;

            for (int i = 0; i < a.resolution_; ++i) {
                s.spectrum[i] = a.spectrum[i] / b;
            }

            return s;
        }

        friend inline Spectrum operator*(Spectrum a, double b)
        {
            Spectrum s;

            for (int i = 0; i < a.resolution_; ++i) {
                s.spectrum[i] = a.spectrum[i] * b;
            }

            return s;
        }

        // サンプル点ランダムに一つ抽出
        inline int getOneSampledPoint()
        {
            Random rnd(static_cast<int>(time(nullptr)));
            return rnd.next(0, int(sample_));
        }

        // 波長の最大値を見つけて返す
        double findMaxSpectrum();

        // 寄与を更新する
        void addContribution(Spectrum weight, Spectrum emitter);

        size_t sample_ = SAMPLE;
        int resolution_ = RESOLUTION;

        // サンプルした波長
        std::vector<int> samplePoints;

        // 実際の波長を保存する
        std::vector<double> spectrum;
    };

    void printSpectrum(Spectrum s);
}

#endif //PATHTRACING_SPECTRUM_HPP
