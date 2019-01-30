//
// Created by 長井亨 on 2019-01-30.
//

#ifndef PATHTRACING_PATHTRACING_HPP
#define PATHTRACING_PATHTRACING_HPP

#include "RenderBase.hpp"

namespace nagato {
    class Pathtracing : public RenderBase {

     public:
        Pathtracing(const Scene &scene, const Film &film, const Camera &camera);

        void render() override {}

     private:
    };
}

#endif //PATHTRACING_PATHTRACING_HPP
