//
// Created by 長井亨 on 2019/01/30.
//

#include "PinholeCamera.hpp"
#include "../core/Random.hpp"

namespace nagato {

    PinholeCamera::PinholeCamera(const Vector3 &eye,
                                 const Vector3 &up,
                                 float f,
                                 int width,
                                 int height) : Camera(eye,
                                                      up, f,
                                                      width,
                                                      height) {
        this->wE = normalize(eye - center);
        this->uE = normalize(cross(up, wE));
        this->vE = cross(wE, uE);
    }

    Ray PinholeCamera::makePrimaryRay(int x, int y) const {
        const auto tf = std::tan(fov * 0.5f);
        const auto rpx = 2.0f * (x + Random::Instance().next()) / width - 1.0f;
        const auto rpy = 2.0f * (y + Random::Instance().next()) / height - 1.0f;
        const auto ww = normalize(Vector3(aspect * tf * rpx, tf * rpy, -1.0f));
        return {eye, uE * ww.x + vE * ww.y + wE * ww.z};
    }


}