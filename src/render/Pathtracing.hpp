//
// Created by 長井亨 on 2019-01-30.
//

#ifndef PATHTRACING_PATHTRACING_HPP
#define PATHTRACING_PATHTRACING_HPP

#include "RenderBase.hpp"

namespace nagato {
    class Pathtracing : public RenderBase {

     public:
        Pathtracing(Scene *scene, Film *film, Camera *camera, int spp, int depth = 5);

        void render() override;

     private:
        Spectrum Li(size_t x, size_t y);

        int spp;
        int maxDepth;
    };
}

#endif //PATHTRACING_PATHTRACING_HPP
