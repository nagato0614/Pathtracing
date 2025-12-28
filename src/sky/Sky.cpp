//
// Created by 長井亨 on 2019-02-04.
//

#include "sky/Sky.hpp"

namespace nagato
{

}
#include "sky/Sky.hpp"
#include <cmath>
#include "core/Random.hpp"

namespace nagato
{
Vector3 Sky::sampleUniformSphere()
{
  auto &rng = Random::Instance();
  const float u = rng.next();
  const float v = rng.next();
  const float z = 1.0f - 2.0f * u;
  const float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
  const float phi = 2.0f * static_cast<float>(M_PI) * v;
  const float x = r * std::cos(phi);
  const float y = r * std::sin(phi);
  return Vector3(x, y, z);
}
} // namespace nagato
