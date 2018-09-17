//
// Created by 長井亨 on 2018/07/18.
//

#include "Random.hpp"

namespace nagato
{

    Random::Random()
    {
        std::random_device rd;
        engine.seed(rd());
        dist.reset();
    }

    float Random::next()
    {
        return dist(engine);
    }

    int Random::nextInt(int from, int to)
    {
        std::uniform_int_distribution<> rand(from, to);
        return rand(engine);
    }

    Random &Random::Instance()
    {
        static Random instance;
        return instance;
    }

    float Random::nextFloat(float from, float to)
    {
        std::uniform_real_distribution<float> rand(from, to);
        return rand(engine);
    }
}
