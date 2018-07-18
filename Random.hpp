//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_RANDOM_HPP
#define PATHTRACING_RANDOM_HPP


#include <random>

namespace nagato {
    class Random {
    public:
        std::mt19937 engine;
        std::uniform_real_distribution<double> dist;

        Random() {};

        Random(int seed);

        double next();
    };
}

#endif //PATHTRACING_RANDOM_HPP
