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
     */
    class Spectrum
    {
     public:
        explicit Spectrum(int resolution, int sample);

        // 設定したサンプル数だけランダムにサンプル点を決める
        void sample();

        friend inline Spectrum operator*(Spectrum a, Spectrum b)
        {
            Spectrum s(a.getResolution(), a.getSampleNum());

            if (a.getResolution() != b.getResolution()) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.getResolution();
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] * b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator+(Spectrum a, Spectrum b)
        {
            Spectrum s(a.getResolution(), a.getSampleNum());

            if (a.getResolution() != b.getResolution()) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.getResolution();
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] + b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator-(Spectrum a, Spectrum b)
        {
            Spectrum s(a.getResolution(), a.getSampleNum());

            if (a.getResolution() != b.getResolution()) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.getResolution();
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] - b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator/(Spectrum a, Spectrum b)
        {
            Spectrum s(a.getResolution(), a.getSampleNum());

            if (a.getResolution() != b.getResolution()) {
                std::cerr << "波長解像度が違います" << std::endl;
                exit(-2);
            }

            auto size = a.getResolution();
            for (int i = 0; i < size; ++i) {
                s.spectrum[i] = a.spectrum[i] / b.spectrum[i];
            }
            return s;

        }

        friend inline Spectrum operator/(Spectrum a, double b)
        {
            Spectrum s(a.getResolution(), a.getSampleNum());

            auto size = a.getResolution();
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


        inline int getSampleNum()
        {
            return sample_;
        }

        inline int getResolution()
        {
            return resolution_;
        }

     private:
        int sample_;
        int resolution_;

        // サンプルした波長
        std::vector<int> samplePoints;

        // 実際の波長を保存する
        std::vector<double> spectrum;
    };
}

#endif //PATHTRACING_SPECTRUM_HPP
