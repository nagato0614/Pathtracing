//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_BVH_HPP
#define PATHTRACING_BVH_HPP

#include "Aabb.hpp"
#include "../object/Triangle.hpp"

namespace nagato
{
    // BVHを構築するノード
    struct BVHNode {
        Aabb bbox;
        int children[2];
        Object *object{};
    };

    // BVHを扱うクラス
    class BVH
    {
     public:
        explicit BVH(std::vector<Object *> *o);

        void constructBVH();

     private:

        void constructBVH_internal();
        float surfaceArea(Aabb bbox);

        Aabb mergeAABB(Aabb a, Aabb b);

        BVHNode root;
        std::vector<Aabb> aabbList;
        std::vector<Object *> *objects;
    };
}


#endif //PATHTRACING_BVH_HPP
