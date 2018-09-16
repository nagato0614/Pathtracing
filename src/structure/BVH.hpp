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
    struct BVHNode
    {
        Aabb bbox;
        Object *object = nullptr;
        int left = -1;
        int right = -1;
    };

    // BVHを扱うクラス
    class BVH
    {
     public:
        BVH();

        explicit BVH(std::vector<Object *> objects);

        void setObject(std::vector<Object *> objects);

        // BVHの構築を始める
        void constructBVH();

        // BVHとレイの交差判定
        std::optional<Hit> intersect(Ray &ray, float min, float max);

        // BVHのノード数を返す
        int getNodeCount();

        // BVHで使用しているメモリサイズを返す[MB]
        size_t getMemorySize();

        // デバッグ用
        void showBVH();

        // デバッグ用 : ノード内にあるオブジェクトが正しいか比較する
        std::optional<Hit> testIntersect(Ray &ray, float min, float max);

        BVHNode *getRoot() const;

        const std::vector<Object *> &getObjects() const;

        const BVHNode *getNodes() const;

     private:

        // BVH構築を再帰的に行う
        void constructBVH_internal(std::vector<Object *> objects, int splitAxis, int nodeIndex);

        // BVHとレイの交差判定を再帰的に行う
        std::optional<Hit> intersect_internal(Ray &ray, float min, float max, int nodeIndex);

        // 新しいノードを作成する
        int makeNewNode();

        float surfaceArea(Aabb bbox);

        Aabb mergeAABB(Aabb a, Aabb b);

        BVHNode *root = nullptr;
        std::vector<Object *> objects;
        int nodeCount = 0;
        BVHNode nodes[BVH_NODE];
    };
}


#endif //PATHTRACING_BVH_HPP
