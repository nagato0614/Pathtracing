//
// Created by 長井亨 on 2018/09/06.
//

#include "Lambert.hpp"

namespace nagato
{
    Lambert::Lambert(Material *m) : BSDF(m)
    {

    }

    Spectrum Lambert::makeNewDirection(int *wavelengthIndex, Vector3 *newDirection, Ray &ray, Hit &surfaceInfo)
    {
        // Sample direction in local coordinates
        const auto n =
                dot(surfaceInfo.normal, -ray.direction) > 0 ? surfaceInfo.normal : -surfaceInfo.normal;
        const auto&[u, v] = tangentSpace(n);
        const auto d = [&]() {
            const auto r = sqrt(Random::Instance().next());
            const auto t = 2 * M_PI * Random::Instance().next();
            const auto x = r * cos(t);
            const auto y = r * sin(t);
            return Vector3((float) x, (float) y, std::sqrt(std::max(float(0.0), static_cast<const float &>(
                    1.0 - x * x - y * y))));
        }();
        // Convert to world coordinates
        *newDirection = u * d.x + v * d.y + n * d.z;
        return material->color;
    }
}