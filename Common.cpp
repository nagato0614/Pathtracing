//
// Created by 長井亨 on 2018/07/18.
//

#include <cmath>
#include <random>
#include <algorithm>
#include "Common.hpp"
#include "Random.hpp"

namespace nagato {

    std::tuple<Vector3, Vector3> tangentSpace(const Vector3 &n) {
      const double s = std::copysign(1, n.z);
      const double a = -1 / (s + n.z);
      const double b = n.x * n.y * a;
      return {
              Vector3(1 + s * n.x * n.x * a, s * b, -s * n.x),
              Vector3(b, s + n.y * n.y * a, -n.y)
      };
    }

    int tonemap(double v) {
      return std::min(
              std::max(int(std::pow(v, 1 / 2.2) * 255), 0), 255);
    }

    int clamp(double v) {
      return std::min(std::max(0, int(v)), 255);
    }

    std::vector<int> make_rand_array_unique(const size_t size, int rand_min, int rand_max)
    {
        if(rand_min > rand_max) std::swap(rand_min, rand_max);
        const auto max_min_diff = (rand_max - rand_min + 1);
        if(max_min_diff < size)
        {
            std::cout << "引数が異常です" << std::endl;
            std::cout << "size : " << size << std::endl;
            std::cout << "rand_min : " << rand_min << std::endl;
            std::cout << "rand_max : " << rand_max << std::endl;

            exit(2);
        }

        std::vector<int> tmp;
        std::mt19937 engine(static_cast<unsigned int>(time(NULL)));
        std::uniform_int_distribution<int> distribution(rand_min, rand_max);

        const auto make_size = static_cast<size_t>(size*1.2);

        while(tmp.size() < size){
            while(tmp.size() < make_size) tmp.push_back(distribution(engine));
            std::sort(tmp.begin(), tmp.end());
            auto unique_end = std::unique(tmp.begin(), tmp.end());

            if(size < std::distance(tmp.begin(), unique_end)){
                unique_end = std::next(tmp.begin(), size);
            }
            tmp.erase(unique_end, tmp.end());
        }

        std::shuffle(tmp.begin(), tmp.end(), engine);
        return std::move(tmp);
    }

    std::string getNowTimeString()
    {
        char buff[] = "";
        time_t now = time(NULL);
        struct tm *pnow = localtime(&now);
        sprintf(buff, "%04d%02d%02d%02d%02d", pnow->tm_year + 1900, pnow->tm_mon + 1, pnow->tm_mday,
                pnow->tm_hour, pnow->tm_min);
        return std::string(buff);
    }
}