//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_TRIANGLE_HPP
#define PATHTRACING_TRIANGLE_HPP


#include "Object.hpp"

namespace nagato
{
    class Triangle : public Object
    {
     public:
        Triangle(Material *m, std::vector<Vector3> p);

        std::optional<Hit> intersect(Ray &ray, float tmin, float tmax) override;

        Aabb getAABB() override ;

        Hit pointSampling(Hit surfaceInfo) override ;


     private:
        std::vector<Vector3> points;
    };
}

#endif //PATHTRACING_TRIANGLE_HPP
