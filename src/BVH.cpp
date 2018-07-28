//
// Created by 長井亨 on 2018/07/28.
//

#include "BVH.hpp"

namespace nagato
{

    BVH::BVH(std::vector<Object *> *o) : objects(o)
    {
        Aabb bbox;

        for (auto i : *o) {
            bbox = mergeAABB(bbox, i->getAABB());
        }

        root.bbox = bbox;
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
    }

    void BVH::constructBVH_internal()
    {

    }
}