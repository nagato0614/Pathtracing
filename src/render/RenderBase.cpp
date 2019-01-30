//
// Created by 長井亨 on 2019-01-30.
//

#include "RenderBase.hpp"
#include "../camera/Camera.hpp"

namespace nagato {

    RenderBase::RenderBase(Scene *scene, Film *film, Camera *camera)
            : scene(scene), film(film), camera(camera) {

    }

    const Film &RenderBase::getFilm() const {
        return *film;
    }
}