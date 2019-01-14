//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_OBJECT_HPP
#define PATHTRACING_OBJECT_HPP


#include <optional>
#include "../core/Ray.hpp"
#include "../color/Spectrum.hpp"
#include "../core/SurfaceType.hpp"
#include "../core/Hit.hpp"
#include "../material/Material.hpp"
#include "../structure/Aabb.hpp"

namespace nagato {

    class Hit;
    class Material;
    class Object {
    public:

        explicit Object(Material *m);

        virtual std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);

        virtual Aabb getAABB() const;

        virtual Hit pointSampling(Hit surfaceInfo) = 0;

        virtual std::string toString() const;

        // オブジェクトの表面積を計算する
        virtual float area() const = 0;

        Material *getMaterial() const ;

    protected:
        Material *material;
    };
}

#endif //PATHTRACING_OBJECT_HPP
