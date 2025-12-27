//
// Created by 長井亨 on 2019-02-04.
//

#include "sky/UniformSky.hpp"

namespace nagato
{

Spectrum UniformSky::getRadiance(const Ray &ray) const { return skyColor; }

UniformSky::UniformSky(const Spectrum &s) : skyColor(s) {}
} // namespace nagato
