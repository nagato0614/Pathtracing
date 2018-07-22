//
// Created by 長井亨 on 2018/07/18.
//

#include "Random.hpp"

namespace nagato {

    Random::Random(int seed) {
      engine.seed(static_cast<unsigned int>(seed));
      dist.reset();
    }

    float Random::next() { return dist(engine); }

    int Random::next(int from, int to)
    {
        std::uniform_int_distribution<> rand(from, to);
        return rand(engine);
    }
}
