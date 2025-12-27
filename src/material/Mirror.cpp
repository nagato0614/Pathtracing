//
// Created by 長井亨 on 2019/01/29.
//

#include "material/Mirror.hpp"
#include "BSDF/Specular.hpp"

namespace nagato
{

Mirror::Mirror(Spectrum c) : Material(SurfaceType::Mirror, c, Spectrum(), 0)
{
  this->bsdf = createSpecular(c);
}
} // namespace nagato
