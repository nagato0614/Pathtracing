//
// Created by 長井亨 on 2018/09/06.
//

#ifndef PATHTRACING_LAMBERT_HPP
#define PATHTRACING_LAMBERT_HPP


#include "BSDF.hpp"

namespace nagato
{
    /**
     * 完全拡散面を扱うクラス
     */
    class Lambert : public BSDF
    {
     public:
        explicit Lambert(Material *m);

        Spectrum makeNewDirection(
                int *wavelengthIndex,
                Vector3 *newDirection,
                Ray &ray,
                const Hit &surfaceInfo) const override;
    };
}

#endif //PATHTRACING_LAMBERT_HPP
