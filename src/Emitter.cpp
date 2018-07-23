//
// Created by 長井亨 on 2018/07/23.
//

#include "Emitter.hpp"
namespace nagato {
    Emitter::Emitter(SurfaceType t, Spectrum c, Spectrum e) : Material(t, c), emittence(e)
    {

    }
}
