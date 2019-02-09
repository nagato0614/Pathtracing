//
// Created by 長井亨 on 2018/07/19.
//

#ifndef PATHTRACING_COLORRGB_HPP
#define PATHTRACING_COLORRGB_HPP


#include "Spectrum.hpp"

namespace nagato
{
    class Spectrum;
    class ColorRGB
    {
     public:
        explicit ColorRGB(float color = 0.0);

        ColorRGB(float r, float g, float b);

        const float operator[](int i) const;

        float &operator[](int i);

        void spectrum2rgb(Spectrum s, Spectrum xbar, Spectrum y, Spectrum z);

        void normilize();

        // rgbの最大値が1.0になるように調整する
        void scale();

        // rgbの中で最大値を求める
        float max();

        // [0.0, 1.0]の範囲に収める
        void clamp();

        // ガンマ補間を行う
        void gammaCorrection(float inverse_gamma);

        // rgbの合計値
        float sum();

        // [0, 255]の各色を渡す
        int r255() const;
        int g255() const;
        int b255() const;

        float r;
        float g;
        float b;
    };
};


#endif //PATHTRACING_COLORRGB_HPP
