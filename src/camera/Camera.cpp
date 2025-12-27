//
// Created by 長井亨 on 2019/01/30.
//

#include "camera/Camera.hpp"

namespace nagato
{
Camera::Camera(const Vector3 &e, const Vector3 &target, const Vector3 &up, float f, int width, int height) :
    eye(e), center(target), up(up), fov(f), aspect(float(width) / height),
    width(width), height(height)
{
}

} // namespace nagato
