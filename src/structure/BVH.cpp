//
// Created by 長井亨 on 2018/07/28.
//

#include "BVH.hpp"
#include <algorithm>
#include <queue>

namespace nagato
{

    BVH::BVH(std::vector<Object *> &o) : objects(o)
    {
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
        root = constructBVH_internal(objects, 0);
    }

    BVHNode *BVH::constructBVH_internal(std::vector<Object *> &objects, int splitAxis)
    {
        if (objects.size() == 1) {
            auto *node = new BVHNode;
            node->object = objects[0];
            return node;
        } else if (objects.empty()) {
            std::cout << "empty" << std::endl;
            return nullptr;
        }

        auto *node = new BVHNode;

        // 大きなAABBの構築
        for (auto i : objects) {
            node->bbox = mergeAABB(node->bbox, i->getAABB());
        }

        std::vector<Object *> left(0);
        std::vector<Object *> right(0);

        std::sort(objects.begin(), objects.end(), [splitAxis](const Object *a, const Object *b) {
            auto aCenter = a->getAABB().getCenter();
            auto bCenter = b->getAABB().getCenter();
            return aCenter[splitAxis] < bCenter[splitAxis];
        });

        for (int i = 0; i < objects.size(); i++) {
            if (i < objects.size() / 2)
                left.push_back(objects[i]);
            else
                right.push_back(objects[i]);
        }

        node->left = constructBVH_internal(left, (splitAxis + 1) % 3);
        node->right = constructBVH_internal(right, (splitAxis + 1) % 3);

        return node;
    }

    void BVH::clearBVH()
    {
        if (root == nullptr)
            return;

        clearBVH_internal(root);
    }

    void BVH::clearBVH_internal(BVHNode *node)
    {
        if (node->right != nullptr)
            clearBVH_internal(node->right);
        else if (node->left != nullptr)
            clearBVH_internal(node->left);

        delete node;
    }

    void BVH::showBVH()
    {
        if (root == nullptr)
            return;

        std::queue<BVHNode *> nodes;
        nodes.push(root);

        for (int i = 0; !nodes.empty(); i++) {
            auto *node = nodes.front();

            printf("[%03d]", i);
            if (!node->object) {
                printf("leaf");
            } else {
                printf("%s\n", typeid(node->object).name());
                std::cout << node->object->toString();
            }

            if (node->right)
                nodes.push(node->right);
            if (node->left)
                nodes.push(node->left);

            printf("\n");
            nodes.pop();
        }
    }

    std::optional<Hit> BVH::intersect(Ray &ray, float min, float max)
    {
        if (root == nullptr) {
            fprintf(stderr, "ERROR : [BVH] BVHを構築していないかオブジェクトがありません");
            exit(-1);
        }

        std::optional<Hit> minh;
        std::queue<BVHNode *> nodes;
        nodes.push(root);

        do {
            auto node = nodes.front();
            if (node->bbox.intersect(ray)) {
                if (node->object == nullptr) {
                    if (node->left != nullptr)
                        nodes.push(node->left);
                    if (node->right != nullptr)
                        nodes.push(node->right);
                } else {
                    auto h = node->object->intersect(ray, min, max);
                    if (h) {
                        minh = h;
                        max = minh->distance;
                    }
                }
            }
            nodes.pop();
        } while (!nodes.empty());

        return minh;
//        return intersect_internal(ray, min, max, root);
    }

    std::optional<Hit> BVH::intersect_internal(Ray &ray, float min, float max, BVHNode *node)
    {
        // AABBとレイが当たるかどうか判定
        if (node->bbox.intersect(ray)) {

            // nodeがオブジェクトを持っているか判定, 持っていないということは中間node
            if (node->object == nullptr) {
                auto right = intersect_internal(ray, min, max, node->right);
                auto left = intersect_internal(ray, min, max, node->left);

                if (right && left) {
                    if (right->distance < left->distance) {
                        return right;
                    } else {
                        return left;
                    }
                } else if (right) {
                    return right;
                } else if (left) {
                    return left;
                } else {
                    return {};
                }
            } else {
                return node->object->intersect(ray, min, max);
            }
        } else {
            return {};
        }
    }
}