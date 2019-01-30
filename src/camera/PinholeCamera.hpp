//
// Created by 長井亨 on 2019/01/30.
//

#ifndef PATHTRACING_PINHOLECAMERA_HPP
#define PATHTRACING_PINHOLECAMERA_HPP

#include "Camera.hpp"

namespace nagato {
    class PinholeCamera : public Camera {
     public:
        PinholeCamera(const Vector3 &eye, const Vector3 &up, float f, int width, int height);

        Ray makePrimaryRay(int x, int y) const override;

     private:
        Vector3 wE;
        Vector3 uE;
        Vector3 vE;

    };
}

#endif //PATHTRACING_PINHOLECAMERA_HPP
