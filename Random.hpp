//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_RANDOM_HPPw
#define PATHTRACING_RANDOM_HPP


#include <random>

namespace nagato {
    class Random {
    public:
        std::mt19937 engine;
        std::uniform_real_distribution<float> dist;

        Random() {};

        Random(int seed);

        float next();

        int next(int from, int to);
    };
}

#endif //PATHTRACING_RANDOM_HPP
