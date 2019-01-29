//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SURFACETYPE_HPP
#define PATHTRACING_SURFACETYPE_HPP
namespace nagato {

    enum class SurfaceType : int {
        Diffuse,
        Mirror,
        Fresnel,
        Emitter,
    };

}
#endif //PATHTRACING_SURFACETYPE_HPP
