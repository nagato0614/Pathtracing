#include <memory>

//
// Created by 長井亨 on 2019/01/14.
//
#include "Film.hpp"
#include "../core/Constant.hpp"
#include <png++/png.hpp>

namespace nagato {

    Film::Film(size_t width, size_t height) : width(width), height(height) {
        this->size = width * height;
        data = std::make_unique<Spectrum[]>(size);
    }

    const Spectrum &Film::operator[](std::size_t index) const {
        return data[index];
    }

    Spectrum &Film::operator[](std::size_t index) {
        return data[index];
    }

    const size_t Film::getWidth() const {
        return width;
    }

    const size_t Film::getHeight() const {
        return height;
    }

    void Film::addSample(const Spectrum &s, size_t x, size_t y) {
        auto index = toIndex(x, y, width, height);
        data[index] = s;
    }

    void Film::addSample(const Spectrum &s, size_t index) {
        data[index] = s;
    }

    const Spectrum &Film::getPixel(size_t x, size_t y) const {
        auto index = toIndex(x, y, width , height);
        return data[index];
    }

    void Film::outputImage(const std::string &filename) const {
        auto rgb_list = this->toRGB();

        png::image< png::rgb_pixel > image{width, height};

        for (size_t i = 0; i < size; i++) {
            png::rgb_pixel pixel;
            pixel.red   = static_cast<png::byte>(std::floor(255.0f * rgb_list[i].x));
            pixel.blue  = static_cast<png::byte>(std::floor(255.0f * rgb_list[i].y));
            pixel.green = static_cast<png::byte>(std::floor(255.0f * rgb_list[i].z));

            auto [x, y] = toXY(i, width, height);
            image.set_pixel(x, y, pixel);
        }

        image.write(filename);
    }

    std::unique_ptr<Vector3[]> Film::toRGB() const {
        std::unique_ptr<Vector3[]> rgb_list(new Vector3[size]);

        const auto k = 1.0f / ybar.sum();

        for (int i = 0; i < size; i++) {
            auto &s = data[i / 3];

            // スペクトルからxyz表色系に変換
            Vector3 xyz;
            xyz.x = (xbar * s).sum() * k;
            xyz.y = (ybar * s).sum() * k;
            xyz.z = (zbar * s).sum() * k;

            if (xyz.sum() == 0 || xyz.y == 0.0) {
                rgb_list[i].x = 0;
                rgb_list[i].y = 0;
                rgb_list[i].z = 0;
                continue;
            }

            // Yxy表色系に変換
            auto Y = xyz.y;
            auto x = xyz.x / xyz.sum();
            auto y = xyz.y / xyz.sum();

            Y = clamp(0.0, 1.0, Y / (1.0f + Y));

            xyz.x = (Y / y) * x;
            xyz.y = Y;
            xyz.z = (Y / y) * (1.0f - x - y);

            // XYZからRGBに変換
            Vector3 rgb;

            rgb.x = xyz.x * toRGB_MATRIX[0][0] + xyz.y * toRGB_MATRIX[0][1] + xyz.z * toRGB_MATRIX[0][2];
            rgb.y = xyz.x * toRGB_MATRIX[1][0] + xyz.y * toRGB_MATRIX[1][1] + xyz.z * toRGB_MATRIX[1][2];
            rgb.z = xyz.x * toRGB_MATRIX[2][0] + xyz.y * toRGB_MATRIX[2][1] + xyz.z * toRGB_MATRIX[2][2];

            if (1.0 < rgb.max()) {
                rgb.normlizeMaxValue();
            }

            // 線形圧縮
            rgb.x = clamp(0.0, 1.0, rgb.x);
            rgb.y = clamp(0.0, 1.0, rgb.y);
            rgb.z = clamp(0.0, 1.0, rgb.z);

            // ガンマ保管
            rgb.x = std::pow(rgb.x, (1.0f / 2.2f));
            rgb.y = std::pow(rgb.y, (1.0f / 2.2f));
            rgb.z = std::pow(rgb.z, (1.0f / 2.2f));

            rgb_list[i].x = rgb.x;
            rgb_list[i].y = rgb.y;
            rgb_list[i].z = rgb.z;
        }

        return rgb_list;
    }

    const size_t Film::getPixelSize() const {
        return width * height;
    }
}