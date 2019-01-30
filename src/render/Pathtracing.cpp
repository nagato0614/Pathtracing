//
// Created by 長井亨 on 2019-01-30.
//

#include "Pathtracing.hpp"

namespace nagato {

    Pathtracing::Pathtracing(const Scene &scene, const Film &film, const Camera &camera)
            : RenderBase(scene, film, camera) {

    }
}