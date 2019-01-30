//
// Created by 長井亨 on 2019/01/30.
//

#ifndef PATHTRACING_CAMERA_HPP
#define PATHTRACING_CAMERA_HPP

#include "../linearAlgebra/Vector3.hpp"
#include "../core/Ray.hpp"

namespace nagato {
    class Camera {
     public:

        /**
         *
         * @param e         カメラの位置
         * @param up        カメラの上方向
         * @param f         視野角
         * @param width     スクリーンの横幅
         * @param height    スクリーンの高さ
         */
        Camera(const Vector3 &e, const Vector3 &up, float f, int width, int height);

        /**
         * 指定したピクセル座標に対する一次レイを生成する
         * @param x
         * @param y
         * @return
         */
        virtual Ray makePrimaryRay(int x, int y) const = 0;

     protected:
        /**
         * カメラ位置
         */
        Vector3 eye;

        /**
         * 投影面の中央
         */
        Vector3 center;

        /**
         * カメラの上方向
         */
        Vector3 up;

        /**
         * 視野角(field of view)
         */
        float fov;

        /**
         * スクリーンの縦横比
         */
        float aspect;

        /**
         * スクリーンの横幅
         */
         int width;

        /**
         * スクリーンの高さ
         */
         int height;
    };
}

#endif //PATHTRACING_CAMERA_HPP
