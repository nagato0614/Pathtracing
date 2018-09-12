//
// Created by 長井亨 on 2018/07/28.
//

#include "BVH.hpp"
#include <algorithm>
#include <queue>

namespace nagato
{

    BVH::BVH(std::vector<Object *> objects)
    {
        this->objects.clear();
        for (Object *o : objects) {
            this->objects.push_back(o);
        }
    }

    float BVH::surfaceArea(const Aabb bbox)
    {
        return 2 * (bbox.max - bbox.min).norm();
    }

    Aabb BVH::mergeAABB(const Aabb a, const Aabb b)
    {
        Aabb bbox;

        bbox.min.x = std::min(a.min.x, b.min.x);
        bbox.min.y = std::min(a.min.y, b.min.y);
        bbox.min.z = std::min(a.min.z, b.min.z);
        bbox.max.x = std::max(a.max.x, b.max.x);
        bbox.max.y = std::max(a.max.y, b.max.y);
        bbox.max.z = std::max(a.max.z, b.max.z);

        return bbox;
    }

    void BVH::constructBVH()
    {
        constructBVH_internal(objects, 0, 0);
    }

    void BVH::constructBVH_internal(std::vector<Object *> objects, int splitAxis, int nodeIndex)
    {
//        std::cout << "NodeIndex  : " << nodeIndex << std::endl;
//        std::cout << "Object num : " << objects.size() << "\n\n";

        // オブジェクトが１つだけの場合それを葉する
        if (objects.size() == 1) {
            nodes[nodeIndex].object = objects.front();
            nodes[nodeIndex].left = -1;
            nodes[nodeIndex].right = -1;
//            std::cout << nodes[nodeIndex].object->toString() << std::endl;
            return;
        } else if (objects.empty()) {
            std::cerr << "empty" << std::endl;
            exit(EMPTY_OBJECT);
        }

        auto *node = &nodes[nodeIndex];

        // 大きなAABBの構築
        for (auto i : objects) {
            node->bbox = mergeAABB(node->bbox, i->getAABB());
        }

        std::vector<Object *> left(0);
        std::vector<Object *> right(0);

        // オブジェクトをsplitAxisで指定する軸についてソートする
        std::sort(objects.begin(), objects.end(), [splitAxis](const Object *a, const Object *b) {
            auto aCenter = a->getAABB().getCenter();
            auto bCenter = b->getAABB().getCenter();
            return aCenter[splitAxis] < bCenter[splitAxis];
        });

        // オブジェクトの分割
        for (int i = 0; i < objects.size(); i++) {
            if (i < objects.size() / 2)
                left.push_back(objects[i]);
            else
                right.push_back(objects[i]);
        }

        // 子を作成
        node->left = nodeCount++;
        node->right = nodeCount++;

        constructBVH_internal(left, (splitAxis + 1) % 3, node->left);
        constructBVH_internal(right, (splitAxis + 1) % 3, node->right);
    }

    void BVH::showBVH()
    {
        if (nodeCount == 0) {
            printf("empty");
            return;
        }
        for (int i = 0; i < nodeCount; i++) {
            printf("%03d : ", i);
            if (nodes[i].object != nullptr) {
                std::cout << nodes[i].object->toString() << std::endl;
            } else {
                printf("[node]\n");
            }
        }
    }

    std::optional<Hit> BVH::intersect(Ray &ray, float min, float max)
    {
        if (nodeCount == 0) {
            fprintf(stderr, "ERROR : [BVH] BVHを構築していないかオブジェクトがありません");
            exit(-1);
        }

        return intersect_internal(ray, min, max, 0);
    }

    std::optional<Hit> BVH::intersect_internal(Ray &ray, float min, float max, int nodeIndex)
    {
        auto *node = &nodes[nodeIndex];
        // rayとaabbとの交差判定
        if (node->bbox.intersect(ray)) {
            // 中間ノードか調べる
            if (node->object == nullptr) {
                // 中間ノードなので更に探索

                std::optional<Hit> right = std::nullopt;
                std::optional<Hit> left = std::nullopt;
                if (node->right != -1) {
                    right = intersect_internal(ray, min, max, node->right);
                }
                if (node->left != -1) {
                    left = intersect_internal(ray, min, max, node->left);
                }
//                if (right)
//                    std::cout << "right : " << right->distance << std::endl;
//                if (left)
//                    std::cout << "left : " << left->distance << std::endl;

                if (!right && !left)
                    return std::nullopt;
                if (right && !left)
                    return right;
                if (!right && left)
                    return left;
                return right->distance < left->distance ? right : left;

            } else {
                return node->object->intersect(ray, min, max);
            }
        } else {
            // ヒットしてない場合
            return std::nullopt;
        }
    }

    int BVH::getNodeCount()
    {
        return nodeCount;
    }

    size_t BVH::getMemorySize()
    {
        return static_cast<size_t>(sizeof(BVHNode) * BVH_NODE * 10e-6);
    }
}