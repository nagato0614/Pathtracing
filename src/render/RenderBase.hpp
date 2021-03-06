//
// Created by 長井亨 on 2019-01-30.
//

#ifndef PATHTRACING_RENDERBASE_HPP
#define PATHTRACING_RENDERBASE_HPP

#include "../core/Scene.hpp"
#include "../film/Film.hpp"
#include "../camera/Camera.hpp"

namespace nagato {
    class RenderBase {
     public:
        RenderBase(Scene *scene, Film *film, Camera *camera);

        virtual void render() = 0;

        const Film &getFilm() const;

     protected:
        Scene *scene;
        Film *film;
        Camera *camera;
    };
}

#endif //PATHTRACING_RENDERBASE_HPP
