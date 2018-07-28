//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_BVH_HPP
#define PATHTRACING_BVH_HPP

#include "Aabb.hpp"
#include "Triangle.hpp"

namespace nagato
{
    // BVHを構築するノード
    struct BVHNode {
        Aabb bbox;
        int children[2]{};
        Object *object{};
    };

    // BVHを扱うクラス
    class BVH
    {
     public:
        BVH(std::vector<Object *> *o);

     private:
        BVHNode root;
        std::vector<Object *> *objects;
    };
}


#endif //PATHTRACING_BVH_HPP
