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
        data[index] = data[index] + s;
    }

    void Film::addSample(const Spectrum &s, size_t index) {
        data[index] = data[index] + s;
    }

    const Spectrum &Film::getPixel(size_t x, size_t y) const {
        auto index = toIndex(x, y, width, height);
        return data[index];
    }

    void Film::outputImage(const std::string &filename) const {
        auto rgb_list = this->toRGB();

        png::image<png::rgb_pixel> image{width, height};

        for (size_t i = 0; i < size; i++) {
            const auto &rgbColor = rgb_list[i];
            png::rgb_pixel pixel;
            pixel.red = static_cast<png::byte>(rgbColor.r255());
            pixel.green = static_cast<png::byte>(rgbColor.g255());
            pixel.blue = static_cast<png::byte>(rgbColor.b255());

            auto[x, y] = toXY(i, width, height);
            image[y][x] = pixel;
        }

        image.write(filename.c_str());
    }

    std::unique_ptr<ColorRGB[]> Film::toRGB() const {
        std::unique_ptr<ColorRGB[]> rgb_list(new ColorRGB[size]);


        for (int i = 0; i < size; i++) {
            ColorRGB pixel;
            pixel.spectrum2rgb(data[i], xbar, ybar, zbar);
            rgb_list[i] = pixel;
        }

        return rgb_list;
    }

    const size_t Film::getPixelSize() const {
        return width * height;
    }
}