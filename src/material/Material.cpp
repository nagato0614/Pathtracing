//
// Created by 長井亨 on 2018/07/23.
//

#include "material/Material.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include "color/ColorRGB.hpp"
#include "core/Hit.hpp"

#include "core/stb_image.h"

namespace nagato
{

SurfaceType Material::type() const { return surfaceType; }

Material::Material(SurfaceType t, Spectrum c, Spectrum e, float emitterL) : surfaceType(t), color(c)
{
  this->emitter = e * emitterL;
}

std::string Material::typeName()
{
  std::string str;
  switch (type())
  {
    case SurfaceType::Diffuse:
      str = "Diffuse";
      break;
    case SurfaceType::Mirror:
      str = "Mirror";
      break;
    case SurfaceType::Fresnel:
      str = "Fresnel";
      break;
    case SurfaceType::Emitter:
      str = "Emitter";
      break;
  }

  return str;
}

BSDF &Material::getBSDF() const { return *bsdf; }

Material::~Material() {}

const Spectrum &Material::getColor() const { return color; }

const Spectrum &Material::getEmitter() const { return emitter; }

void Material::setDiffuseTexture(const std::string &path)
{
  diffuseTexturePath = path;
  loadDiffuseTexture(path);
}

bool Material::hasDiffuseTexture() const
{
  return diffuseTextureImage != nullptr && !diffuseTextureImage->texels.empty();
}

const std::string &Material::getDiffuseTexture() const
{
  static const std::string empty;
  if (!diffuseTexturePath)
  {
    return empty;
  }
  return diffuseTexturePath.value();
}

Spectrum Material::evaluateColor(const Hit *hit) const
{
  return color * getTextureColor(hit);
}

Spectrum Material::getTextureColor(const Hit *hit) const
{
  if (!hit || !hit->hasTexcoord() || !diffuseTextureImage)
  {
    return Spectrum(1.0f);
  }
  return sampleTexture(hit->getTexcoord());
}

void Material::loadDiffuseTexture(const std::string &path)
{
  int width = 0;
  int height = 0;
  int comp = 0;
  stbi_uc *data = stbi_load(path.c_str(), &width, &height, &comp, 3);
  if (!data)
  {
    std::cerr << "Failed to load texture: " << path << std::endl;
    diffuseTextureImage.reset();
    return;
  }

  diffuseTextureImage = std::make_unique<TextureImage>();
  diffuseTextureImage->width = width;
  diffuseTextureImage->height = height;
  diffuseTextureImage->texels.resize(static_cast<size_t>(width) * height);

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      const int pixelIndex = (y * width + x) * 3;
      const float r = static_cast<float>(data[pixelIndex]) / 255.0f;
      const float g = static_cast<float>(data[pixelIndex + 1]) / 255.0f;
      const float b = static_cast<float>(data[pixelIndex + 2]) / 255.0f;
      diffuseTextureImage->texels[y * width + x] = Spectrum::rgb2Spectrum(ColorRGB(r, g, b));
    }
  }

  stbi_image_free(data);
}

Spectrum Material::sampleTexture(const Vector2 &uv) const
{
  if (!diffuseTextureImage || diffuseTextureImage->texels.empty())
  {
    return Spectrum(1.0f);
  }

  auto wrap = [](float value)
  {
    float wrapped = std::fmod(value, 1.0f);
    if (wrapped < 0.0f)
    {
      wrapped += 1.0f;
    }
    return wrapped;
  };

  const float u = wrap(uv.x);
  const float v = wrap(uv.y);

  const int x = std::clamp(static_cast<int>(u * diffuseTextureImage->width), 0, diffuseTextureImage->width - 1);
  const int y = std::clamp(static_cast<int>((1.0f - v) * diffuseTextureImage->height), 0, diffuseTextureImage->height - 1);

  return diffuseTextureImage->texels[static_cast<size_t>(y) * diffuseTextureImage->width + x];
}

} // namespace nagato
