//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_HIT_HPP
#define PATHTRACING_HIT_HPP

#include "../object/Object.hpp"

namespace nagato {
    class Object;

    class Hit {
    public:
        Hit();

        Hit(float d, Vector3 p, Vector3 n, Object *sphere);

        const float getDistance() const ;

        const Vector3 &getPoint() const ;

        const Vector3 &getNormal() const ;

        const Object & getObject() const ;
    private:
        float distance;
        Vector3 point;
        Vector3 normal;
        Object *object;
    };
}

#endif //PATHTRACING_HIT_HPP
