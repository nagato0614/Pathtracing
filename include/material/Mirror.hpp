//
// Created by 長井亨 on 2019/01/29.
//

#ifndef PATHTRACING_MIRROR_HPP
#define PATHTRACING_MIRROR_HPP

#include "material/Material.hpp"

namespace nagato
{
class Mirror : public Material
{
  public:
    explicit Mirror(Spectrum c);

  private:
};
} // namespace nagato

#endif // PATHTRACING_MIRROR_HPP
