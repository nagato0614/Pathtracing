//
// Created by kiki on 2018/07/18.
//

#ifndef PATHTRACING_SPECTRUM_HPP
#define PATHTRACING_SPECTRUM_HPP

#include <vector>
#include <iostream>
#include "Random.hpp"

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

        explicit Spectrum(int resolution, int sample);

        // すべての波長をinit_numの値で初期化する
        Spectrum(int resolution, int sample, double init_num);

        // 波長に対する反射率を保存したcsvから読み込む1
        Spectrum(int resolution, int sample, std::string filenam);

        // 設定したサンプル数だけランダムにサンプル点を決める
        void sample();

        friend inline Spectrum operator*(Spectrum a, Spectrum b)
        {
            Spectrum s(a.resolution_, a.sample_);

            if (a.resolution_ != b.resolution_) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.resolution_;
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] * b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator+(Spectrum a, Spectrum b)
        {
            Spectrum s(a.resolution_, a.sample_);

            if (a.resolution_ != b.resolution_) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.resolution_;
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] + b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator-(Spectrum a, Spectrum b)
        {
            Spectrum s(a.resolution_, a.sample_);

            if (a.resolution_ != b.resolution_) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.resolution_;
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] - b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator/(Spectrum a, Spectrum b)
        {
            Spectrum s(a.resolution_, a.sample_);

            if (a.resolution_ != b.resolution_) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.resolution_;
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] / b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator/(Spectrum a, double b)
        {
            Spectrum s(a.resolution_, a.sample_);

            auto size = a.resolution_;
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] / b;
            }

            return s;
        }

        // サンプル点ランダムに一つ抽出
        inline int getOneSampledPoint()
        {
            Random rnd(static_cast<int>(time(nullptr)));
            return rnd.next(0, sample_);
        }

        int sample_;
        int resolution_;

        // サンプルした波長
        std::vector<int> samplePoints;

        // 実際の波長を保存する
        std::vector<double> spectrum;
    };
}

#endif //PATHTRACING_SPECTRUM_HPP
