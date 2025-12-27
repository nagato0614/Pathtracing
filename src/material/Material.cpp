//
// Created by 長井亨 on 2018/07/23.
//

#include "Material.hpp"
#include <utility>

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

} // namespace nagato
