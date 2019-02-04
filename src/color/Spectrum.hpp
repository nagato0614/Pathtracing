//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SPECTRUM_HPP
#define PATHTRACING_SPECTRUM_HPP

#include <vector>
#include <iostream>
#include "../core/Random.hpp"
#include "../core/Common.hpp"
#include "../core/csv.h"

namespace nagato {

    /**
     * サンプリング数
     */
    constexpr int nSamples = 40;

    /**
     * 最小波長
     */
    constexpr int minSpectral = 380;

    /**
     * 最大波長
     */
    constexpr int maxSpectral = 780;


    /**
     * サンプル間隔
     */
    constexpr float resolution = float(maxSpectral - minSpectral) / nSamples;

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

        const float operator[](int i) const;

        float &operator[](int i);

        Spectrum operator+=(const Spectrum &s);

        Spectrum operator+=(float s);

        Spectrum operator-=(const Spectrum &s);

        Spectrum operator-=(float s);

        Spectrum operator*=(const Spectrum &s);

        Spectrum operator*=(float s);

        Spectrum operator/=(const Spectrum &s);

        Spectrum operator/=(float s);

        size_t sample_ = resolution;
        int resolution_ = nSamples;

        // 実際の波長を保存する
        float spectrum[nSamples];
    };

    void printSpectrum(Spectrum s);

    void printSample(Spectrum s);

    Spectrum operator*(const Spectrum &a, const Spectrum &b);

    Spectrum operator+(const Spectrum &a, const Spectrum &b);

    Spectrum operator-(const Spectrum &a, const Spectrum &b);

    Spectrum operator/(const Spectrum &a, const Spectrum &b);

    Spectrum operator/(const Spectrum &a, float b);

    Spectrum operator*(const Spectrum &a, float b);

    Spectrum operator*(float a, const Spectrum &b);

    /**
     * PBRTより
     * http://www.pbr-book.org/3ed-2018/Color_and_Radiometry/The_SampledSpectrum_Class.html#fragment-Advancetofirstrelevantwavelengthsegment-0
     * SPDからSpectrumを作成する
     * 区分線形関数を平均化しサンプルを行う
     * @param lambda    サンプルされた波長
     * @param v         サンプルされた波長の値
     * @param size      サンプル数
     * @return
     */
    Spectrum makeSpectrum(const std::vector<float> &lambda,
                          const std::vector<float> &v,
                          int size);

    /**
     * PBRTより
     * 指定した範囲の波長の寄与をspdから抽出し平均化する
     * @param lambda
     * @param v
     * @param n
     * @param lambdaStart
     * @param lambdaEnd
     * @return
     */
    float averageSpectrumSamples(const std::vector<float> &lambda,
                                 const std::vector<float> &v,
                                 int n,
                                 float lambdaStart,
                                 float lambdaEnd);

    Spectrum loadSPDFile(std::string filename);
}

#endif //PATHTRACING_SPECTRUM_HPP
