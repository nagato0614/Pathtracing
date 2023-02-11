//
// Created by 長井亨 on 2019-01-31.
//

#include <tuple>
#include "linearAlgebra.hpp"

namespace nagato {

    /**
     * 正規直交基底を作る
     * @param v1 基準となるベクトル
     * @return
     */
    std::tuple<Vector3, Vector3, Vector3> orthonormalBasis(const Vector3 &v1) {
        Vector3 v2, v3;
        if (std::abs(v1.x) > 0.9)
            v2 = Vector3{0, 1, 0};
        else
            v2 = Vector3{1, 0, 0};

        v2 = normalize(v2 - dot(v1, v2) * v1);
        v3 = cross(v1, v2);

        return std::make_tuple(v1, v2, v3);
    }

    /**
     * z軸の正方向を法線ベクトルとしてローカル座標系に変換する
     * @param v         変換したいベクトル
     * @param normal    基準となる法線
     * @return
     */
    Vector3 worldToLocal(const Vector3 &v, const Vector3 &normal) {
        auto [n, s, t] = orthonormalBasis(normal);

        return {dot(v, s), dot(v, t), dot(v, t)};
    }

    /**
     * z軸の正方向を法線ベクトルとしてワールド座標系に変換する
     * @param v         変換したいベクトル
     * @param normal    基準となる法線
     * @return
     */
    Vector3 localToWorld(const Vector3 &v, const Vector3 &normal) {
        auto [n, s, t] = orthonormalBasis(normal);

        Vector3 a = {s.x, t.x, n.x};
        Vector3 b = {s.y, t.y, n.y};
        Vector3 c = {s.z, t.z, n.z};

        return {dot(v, a), dot(v, b), dot(v, c)};
    }
}
