#include <memory>

//
// Created by 長井亨 on 2019-02-09.
//

#include "ImageBasedLighting.hpp"

namespace nagato {

    Spectrum ImageBasedLighting::getRadiance(const Ray &ray) const {

        const float u = 1 + M_1_PI * std::atan2(ray.getDirection().x, -ray.getDirection().z);
        const float v = M_1_PI * std::acos(ray.getDirection().y);


        int x = (u / 2.0f) * width;
        int y = v * height;
        int index = x + y * width;

        return image[index];
    }

    ImageBasedLighting::ImageBasedLighting(const std::string &filename) {
        open(filename);
    }

    void ImageBasedLighting::open(const std::string &filename) {
//        Imf::RgbaInputFile file(filename.c_str());
//        Imath::Box2i dw = file.dataWindow();
//
//        width = dw.max.x - dw.min.x + 1;
//        height = dw.max.y - dw.min.y + 1;
//
//        std::vector<Imf::Rgba> pixels(width * height);
//        file.setFrameBuffer(&pixels[0] - dw.min.x - dw.min.y * width, 1, width);
//        file.readPixels(dw.min.y, dw.max.y);

        image = std::make_unique<Spectrum[]>(width * height);

        for (int i = 0; i < height * width; i++) {
//            image.get()[i] = Spectrum::rgb2Spectrum({pixels[i].r, pixels[i].g, pixels[i].b});
        }
    }
}