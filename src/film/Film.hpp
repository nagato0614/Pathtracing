//
// Created by 長井亨 on 2019/01/14.
//

#ifndef PATHTRACING_FILM_H
#define PATHTRACING_FILM_H

#include "../color/Spectrum.hpp"

namespace nagato {
    class Film {
    public:
        /**
         * 生成するフィルムサイズを指定する
         * @param width
         * @param height
         */
        Film(int width, int height);

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

        const int getWidth() const ;

        const int getHeight() const ;

        /**
         * 指定した位置に求めた値を蓄積する
         * @param s
         * @param x
         * @param y
         */
        void addSample(Spectrum &s, int x, int y);

        /**
         * 指定したファイル名の画像を作成する
         * 拡張子がpng以外の場合はpngにする
         * @param filename
         */
        void outputImage(std::string filename) const ;

        const Spectrum &getPixel(int x, int y) const;
    private:
        int width;
        int height;

        std::shared_ptr<Spectrum[]> data;
    };
}

#endif //PATHTRACING_FILM_H
