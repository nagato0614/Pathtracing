//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SPECTRUM_HPP
#define PATHTRACING_SPECTRUM_HPP

#include <iostream>
#include <vector>
#include "core/Common.hpp"
#include "core/Random.hpp"
#include "core/csv.h"
#include "color/ColorRGB.hpp"

namespace nagato
{

class ColorRGB;

class Spectrum;

/**
 * サンプリング数
 */
constexpr int nSamples = 10;

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
Spectrum makeSpectrum(const float *lambda, const float *v, int size);

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
float averageSpectrumSamples(
  const float *lambda, const float *v, int n, float lambdaStart, float lambdaEnd);

/**
 * 指定したファイルからspdを読み込みサンプリングを行う
 * @param filename
 * @return
 */
Spectrum loadSPDFile(std::string filename);

// RGBからSpectrumの変換の際に使用
const int LAMBDA_SIZE = 10;
const float LAMBDA[] = {380, 420, 460, 500, 540, 580, 620, 660, 700, 740};
const float WHITE[] = {1.0, 1.0, 1.0, 0.9999, 0.9993, 0.9992, 0.9998, 1.0, 1.0, 1.0, 1.0, 1.0};
const float CYAN[] = {0.9714, 0.9426, 1.0007, 1.0007, 1.0007, 1.0007, 0.1564, 0.0, 0.0, 0.0};
const float MAGENTA[] = {1.0, 1.0, 0.9685, 0.2229, 0.0, 0.0458, 0.8369, 1.0, 1.0, 0.9959};
const float YELLOW[] = {0.001, 0.0, 0.1008, 0.6651, 1.0, 1.0, 0.9996, 0.9586, 0.9685, 0.9840};
const float RED[] = {0.1012, 0.0515, 0.0, 0.0, 0.0, 0.0, 0.8325, 1.0149, 1.0149, 1.0149};
const float GREEN[] = {0.0, 0.0, 0.0273, 0.7943, 1.0, 0.9418, 0.1719, 0.0, 0.0, 0.0025};
const float BLUE[] = {1.0, 1.0, 0.8916, 0.3323, 0.0, 0.0, 0.0003, 0.0369, 0.0483, 0.0496};

/**!
 * スペクトルを扱うデータ構造
 * 380nm ~ 780nmの可視光範囲のみ扱う前提
 */
class Spectrum
{
  public:
    static void initSpectrum()
    {
      auto lerp = [](float t, float a, float b) { return (1 - t) * a + t * b; };

      rgb2specWhite = makeSpectrum(LAMBDA, WHITE, LAMBDA_SIZE);
      rgb2specCyan = makeSpectrum(LAMBDA, CYAN, LAMBDA_SIZE);
      rgb2specMagenta = makeSpectrum(LAMBDA, MAGENTA, LAMBDA_SIZE);
      rgb2specYellow = makeSpectrum(LAMBDA, YELLOW, LAMBDA_SIZE);
      rgb2specRed = makeSpectrum(LAMBDA, RED, LAMBDA_SIZE);
      rgb2specGreen = makeSpectrum(LAMBDA, GREEN, LAMBDA_SIZE);
      rgb2specBlue = makeSpectrum(LAMBDA, BLUE, LAMBDA_SIZE);
      //            for (int i = 0; i < nSamples; i++) {
      //                float start = lerp(float(i) / nSamples,
      //                                   minSpectral, maxSpectral);
      //                float end = lerp(float(i + 1) / nSamples,
      //                                 minSpectral, maxSpectral);
      //                rgb2specWhite[i] = averageSpectrumSamples(LAMBDA, WHITE, LAMBDA_SIZE, start,
      //                end); rgb2specCyan[i] = averageSpectrumSamples(LAMBDA, CYAN, LAMBDA_SIZE,
      //                start, end); rgb2specMagenta[i] = averageSpectrumSamples(LAMBDA, MAGENTA,
      //                LAMBDA_SIZE, start, end); rgb2specYellow[i] = averageSpectrumSamples(LAMBDA,
      //                YELLOW, LAMBDA_SIZE, start, end); rgb2specRed[i] =
      //                averageSpectrumSamples(LAMBDA, RED, LAMBDA_SIZE, start, end);
      //                rgb2specGreen[i] = averageSpectrumSamples(LAMBDA, GREEN, LAMBDA_SIZE, start,
      //                end); rgb2specBlue[i] = averageSpectrumSamples(LAMBDA, BLUE, LAMBDA_SIZE,
      //                start, end);
      //            }
    }

    // すべての波長をinit_numの値で初期化する
    explicit Spectrum(float init_num = 0.0);

    // 波長に対する反射率を保存したcsvから読み込む1
    explicit Spectrum(std::string filename);

    // サンプル点ランダムに一つ抽出
    inline int getOneSampledPoint() { return Random::Instance().nextInt(0, int(sample_)); }

    /**
     * rgbからSpectrumに変換する
     * @param rgb
     * @return
     */
    static Spectrum rgb2Spectrum(const ColorRGB &rgb);

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

    Spectrum &operator+=(const Spectrum &s);

    Spectrum &operator+=(float s);

    Spectrum &operator-=(const Spectrum &s);

    Spectrum &operator-=(float s);

    Spectrum &operator*=(const Spectrum &s);

    Spectrum &operator*=(float s);

    Spectrum &operator/=(const Spectrum &s);

    Spectrum &operator/=(float s);

    size_t sample_ = resolution;
    int resolution_ = nSamples;

    // 実際の波長を保存する
    float spectrum[nSamples];

  private:
    static Spectrum rgb2specWhite;
    static Spectrum rgb2specCyan;
    static Spectrum rgb2specMagenta;
    static Spectrum rgb2specYellow;
    static Spectrum rgb2specRed;
    static Spectrum rgb2specGreen;
    static Spectrum rgb2specBlue;
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

} // namespace nagato

#endif // PATHTRACING_SPECTRUM_HPP
