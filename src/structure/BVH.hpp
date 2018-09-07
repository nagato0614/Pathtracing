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
        Object *object = nullptr;
        BVHNode *right = nullptr;
        BVHNode *left = nullptr;
    };

    // BVHを扱うクラス
    class BVH
    {
     public:
        explicit BVH(std::vector<Object *> &o);

        // BVHの構築を始める
        void constructBVH();

        // BVHとレイの交差判定
        std::optional<Hit> intersect(Ray &ray, float min, float max);

        // 不要になったBVHを開放する
        void clearBVH();

        // デバッグ用
        void showBVH();
     private:

        // BVH構築を再帰的に行う
        BVHNode *constructBVH_internal(std::vector<Object *> &objects, int splitAxis);

        // BVHのメモリ解放を再帰的に行う
        void clearBVH_internal(BVHNode *node);

        // BVHとレイの交差判定を再帰的に行う
        std::optional<Hit> intersect_internal(Ray &ray, float min, float max, BVHNode *node);

        float surfaceArea(Aabb bbox);

        Aabb mergeAABB(Aabb a, Aabb b);

        BVHNode *root = nullptr;
        std::vector<Object *> &objects;
    };
}


#endif //PATHTRACING_BVH_HPP
