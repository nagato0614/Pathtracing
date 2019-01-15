#include <memory>

//
// Created by 長井亨 on 2019/01/14.
//
#include "Film.hpp"

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

    void Film::outputImage(std::string filename) const {

    }
}