//
// Created by 長井亨 on 2019/01/30.
//

#include "Camera.hpp"

namespace nagato
{
Camera::Camera(const Vector3 &e, const Vector3 &up, float f, int width, int height) :
    eye(e), up(up), fov(f), aspect(float(width) / height), center(eye + Vector3(0, 0, -1)),
    width(width), height(height)
{
}

} // namespace nagato
