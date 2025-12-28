//
// Created by 長井亨 on 2019-01-30.
//

#ifndef PATHTRACING_PATHTRACING_HPP
#define PATHTRACING_PATHTRACING_HPP

#include <string>
#include "render/RenderBase.hpp"

namespace nagato
{
class Pathtracing : public RenderBase
{

  public:
    Pathtracing(Scene *scene, Film *film, Camera *camera, int spp, int depth = 5);

    void render() override;

    void render(const std::string &outputFilename);

    /**
     * @brief 1パス分（1サンプリング分）のレンダリングを行う
     * @param current_pass 現在のパス数（0オリジン）
     */
    void render(int current_pass) override;

  private:
    Spectrum Li(size_t x, size_t y);

    int spp;
    int maxDepth;
};
} // namespace nagato

#endif // PATHTRACING_PATHTRACING_HPP
