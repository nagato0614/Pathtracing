//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_RANDOM_HPP
#define PATHTRACING_RANDOM_HPP

#include <random>

namespace nagato
{
class Random
{
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
     * [from, to]の範囲のランダムな実数値を取得する
     * @param from
     * @param to
     * @return
     */
    [[deprecated("please use nextReal function")]]
    float nextFloat(float from, float to);

    /**
     * [from, to]の範囲のランダムな実数値を取得する
     * @tparam T
     * @param from
     * @param to
     * @return
     */
    template<typename T>
    T nextReal(T from, T to)
    {
      std::uniform_real_distribution<T> rand(from, to);
      return rand(engine);
    }

    static Random &Instance();

  private:
    Random(const Random &) = delete;
    Random &operator=(const Random &) = delete;
    Random(Random &&) = delete;
    Random &operator=(Random &&) = delete;
    Random();

    std::mt19937 engine;
    std::uniform_real_distribution<float> dist;
};
} // namespace nagato

#endif // PATHTRACING_RANDOM_HPP
