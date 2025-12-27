#include <memory>

//
// Created by 長井亨 on 2019/01/14.
//
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#include "../core/Constant.hpp"
#include "Film.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../core/stb_image_write.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace nagato
{

Film::Film(size_t width, size_t height) : width(width), height(height)
{
  this->size = width * height;
  data = std::make_unique<Spectrum[]>(size);
}

const Spectrum &Film::operator[](std::size_t index) const { return data[index]; }

Spectrum &Film::operator[](std::size_t index) { return data[index]; }

const size_t Film::getWidth() const { return width; }

const size_t Film::getHeight() const { return height; }

void Film::addSample(const Spectrum &s, size_t x, size_t y)
{
  auto index = toIndex(x, y, width, height);
  data[index] = data[index] + s;
}

void Film::addSample(const Spectrum &s, size_t index) { data[index] = data[index] + s; }

const Spectrum &Film::getPixel(size_t x, size_t y) const
{
  auto index = toIndex(x, y, width, height);
  return data[index];
}

void Film::outputImage(const std::string &filename) const
{
  auto rgb_list = this->toRGB();

  std::vector<unsigned char> dst(width * height * 4);
  for (size_t i = 0; i < width * height; i++)
  {
    dst[i * 4 + 0] = static_cast<unsigned char>(rgb_list[i].r255());
    dst[i * 4 + 1] = static_cast<unsigned char>(rgb_list[i].g255());
    dst[i * 4 + 2] = static_cast<unsigned char>(rgb_list[i].b255());
    dst[i * 4 + 3] = 255;
  };

  auto ret = stbi_write_png(filename.c_str(),
                            static_cast<int>(width),
                            static_cast<int>(height),
                            4,
                            static_cast<const void *>(dst.data()),
                            static_cast<int>(width * 4));

  if (ret < 0)
    std::cout << "cant out put Image : " << filename << std::endl;
}

std::unique_ptr<ColorRGB[]> Film::toRGB() const
{
  std::unique_ptr<ColorRGB[]> rgb_list(new ColorRGB[size]);

  for (int i = 0; i < size; i++)
  {
    ColorRGB pixel;
    pixel.spectrum2rgb(data[i], xbar, ybar, zbar);
    rgb_list[i] = pixel;
  }

  return rgb_list;
}

const size_t Film::getPixelSize() const { return width * height; }
} // namespace nagato
