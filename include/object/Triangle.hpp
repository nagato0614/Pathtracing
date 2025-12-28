//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_TRIANGLE_HPP
#define PATHTRACING_TRIANGLE_HPP

#include <vector>
#include "linearAlgebra/Vector2.hpp"
#include "object/Object.hpp"

namespace nagato
{
class Triangle : public Object
{
  public:
    Triangle(Material *m, std::vector<Vector3> p, std::vector<Vector2> t, bool hasTexcoord);

    std::optional<Hit> intersect(Ray &ray, float tmin, float tmax) override;

    Aabb getAABB() const override;

    Hit pointSampling(Hit surfaceInfo) override;

    Hit sampleSurfacePoint() const override;

    std::string toString() const override;

    float area() const override;

  private:
    std::vector<Vector3> points;
    std::vector<Vector2> texcoords;
    bool hasTexcoord;
};
} // namespace nagato

#endif // PATHTRACING_TRIANGLE_HPP
