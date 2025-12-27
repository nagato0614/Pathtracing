//
// Created by 長井亨 on 2019/01/14.
//

#ifndef PATHTRACING_FILM_H
#define PATHTRACING_FILM_H

#include <memory>
#include "../color/ColorRGB.hpp"
#include "../color/Spectrum.hpp"

namespace nagato
{
class Film
{
  public:
    /**
     * 生成するフィルムサイズを指定する
     * @param width
     * @param height
     */
    Film(size_t width, size_t height);

    /**
     * 指定したindexのピクセル値を取得
     * @param index
     * @return
     */
    const Spectrum &operator[](std::size_t index) const;

    /**
     * 指定したindexにピクセル値を挿入
     * @param index
     * @return
     */
    Spectrum &operator[](std::size_t index);

    const size_t getWidth() const;

    const size_t getHeight() const;

    const size_t getPixelSize() const;

    /**
     * 指定した位置に求めた値を蓄積する
     * @param s
     * @param x
     * @param y
     */
    void addSample(const Spectrum &s, size_t x, size_t y);

    /**
     * 指定したindexに放射輝度を蓄積する
     * @param s
     * @param index
     */
    void addSample(const Spectrum &s, size_t index);

    /**
     * 指定したファイル名の画像を作成する
     * 拡張子がpng以外の場合はpngにする
     * @param filename
     */
    void outputImage(const std::string &filename) const;

    std::unique_ptr<ColorRGB[]> toRGB() const;

    const Spectrum &getPixel(size_t x, size_t y) const;

  private:
    size_t width;
    size_t height;
    std::size_t size;

    const Spectrum xbar{"../property/cie_sco_2degree_xbar.csv"};
    const Spectrum ybar{"../property/cie_sco_2degree_ybar.csv"};
    const Spectrum zbar{"../property/cie_sco_2degree_zbar.csv"};

    std::unique_ptr<Spectrum[]> data;
};
} // namespace nagato

#endif // PATHTRACING_FILM_H
