//
// Created by 長井亨 on 2019-01-30.
//

#ifndef PATHTRACING_RENDERBASE_HPP
#define PATHTRACING_RENDERBASE_HPP

#include "camera/Camera.hpp"
#include "core/Scene.hpp"
#include "film/Film.hpp"

namespace nagato
{
class RenderBase
{
  public:
    RenderBase(Scene *scene, Film *film, Camera *camera);
    virtual ~RenderBase() = default;

    virtual void render() = 0;

    virtual void render(int current_pass) = 0;

    const Film &getFilm() const;

  protected:
    Scene *scene;
    Film *film;
    Camera *camera;
};
} // namespace nagato

#endif // PATHTRACING_RENDERBASE_HPP
