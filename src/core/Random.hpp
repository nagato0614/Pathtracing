//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_RANDOM_HPPw
#define PATHTRACING_RANDOM_HPP


#include <random>

namespace nagato {
    class Random {
    public:

        float next();

        /**
         * [from,to]の範囲のランダムな整数値を取得する
         * @param from
         * @param to
         * @return
         */
        int nextInt(int from, int to);

        /**
         * [from, to]の範囲のランダムな整数値を取得する
         * @param from
         * @param to
         * @return
         */
        float nextFloat(float from, float to);

        static Random& Instance();
     private:
        Random(const Random&) = delete;
        Random& operator=(const Random&) = delete;
        Random(Random&&) = delete;
        Random& operator=(Random&&) = delete;
        Random();

        std::mt19937 engine;
        std::uniform_real_distribution<float> dist;
    };
}

#endif //PATHTRACING_RANDOM_HPP
