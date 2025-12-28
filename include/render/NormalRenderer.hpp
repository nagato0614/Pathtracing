//
// 法線のみを描画するデバッグ用レンダラー
//

#ifndef PATHTRACING_NORMALRENDERER_HPP
#define PATHTRACING_NORMALRENDERER_HPP

#include <string>

#include "render/RenderBase.hpp"

namespace nagato
{
class NormalRenderer : public RenderBase
{
  public:
    NormalRenderer(Scene *scene, Film *film, Camera *camera);

    void render() override;

    void render(int current_pass) override;

    void render(const std::string &outputFilename);
};
} // namespace nagato

#endif // PATHTRACING_NORMALRENDERER_HPP
