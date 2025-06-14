#pragma once
#include "vectorbase.h"

namespace ym {

    class Vector3;

    class Vector2 : public VectorBase<float> {
    public:
        Vector2() : VectorBase() {}
        Vector2(float x, float y) : VectorBase(x, y) {}

        // メンバ変数
        static Vector2 up;
        static Vector2 down;
        static Vector2 left;
        static Vector2 right;

        static Vector2 zero;
        static Vector2 one;


        // DirectXMath互換
        DirectX::XMFLOAT2 ToXMFLOAT2() const {
            return DirectX::XMFLOAT2(x, y);
        }

        // Vector3に変換
        Vector3 ToVector3(float z = 0.0f) const;
    };
}
