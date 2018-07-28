//
// Created by 長井亨 on 2018/07/18.
//

#include "Object.hpp"

namespace nagato
{

    std::optional<Hit> Object::intersect(Ray &ray, float tmin, float tmax)
    {
        return {};
    }

    Object::Object( Material *m) : material(m)
    {

    }
}
