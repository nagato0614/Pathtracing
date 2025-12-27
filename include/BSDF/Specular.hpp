//
// Created by 長井亨 on 2018/09/06.
//

#ifndef PATHTRACING_SPECULAR_HPP
#define PATHTRACING_SPECULAR_HPP

#include "BSDF/BSDF.hpp"

namespace nagato
{
class Specular : public BSDF
{
  public:
    explicit Specular(Spectrum c);

    Spectrum makeNewDirection(int *wavelengthIndex,
                              Vector3 *newDirection,
                              Ray &ray,
                              const Hit &surfaceInfo,
                              float *pdf) const override;
};

std::shared_ptr<Specular> createSpecular(Spectrum c);
} // namespace nagato

#endif // PATHTRACING_SPECULAR_HPP
