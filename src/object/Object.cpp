//
// Created by 長井亨 on 2018/07/18.
//

#include "object/Object.hpp"

namespace nagato
{

std::optional<Hit> Object::intersect(Ray &ray, float tmin, float tmax) { return std::nullopt; }

Object::Object(Material *m) : material(m) {}

Aabb Object::getAABB() const { return Aabb(nagato::Vector3(), nagato::Vector3()); }

std::string Object::toString() const { return "Object : " + material->typeName(); }

const Material &Object::getMaterial() const { return *material; }
} // namespace nagato
