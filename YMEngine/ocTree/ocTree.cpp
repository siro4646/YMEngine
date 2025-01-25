#include "ocTree.h"
#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"

namespace ym
{
    AABB GetAABB(Object *object) {
        // ローカル空間の8つの頂点を列挙
        auto localTransform = object->localTransform;
        auto globalTransform = object->worldTransform;

        std::vector<Vector3> localVertices = {
            { -0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f, -0.5f },
            { -0.5f,  0.5f, -0.5f },
            {  0.5f,  0.5f, -0.5f },
            { -0.5f, -0.5f,  0.5f },
            {  0.5f, -0.5f,  0.5f },
            { -0.5f,  0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f }
        };

        // ワールド空間での最小点・最大点の初期化
        Vector3 worldMin(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 worldMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        // グローバルスケールと回転行列を取得
        XMMATRIX rotationMatrix = globalTransform.GetMatrixRotation();
        Vector3 globalScale = globalTransform.Scale;

        // 各頂点をワールド空間に変換
        for (const auto &vertex : localVertices) {
            // ローカルスケールを適用
            Vector3 scaledVertex = Vector3(vertex.x * localTransform.Scale.x,
                vertex.y * localTransform.Scale.y,
                vertex.z * localTransform.Scale.z);

            // 回転を適用
            auto toFloat3 = scaledVertex.ToXMFLOAT3();
            XMVECTOR v = XMVector3Transform(XMLoadFloat3(&toFloat3), rotationMatrix);
            Vector3 rotatedVertex(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);

            // グローバルスケールを適用
            rotatedVertex.x *= globalScale.x;
            rotatedVertex.y *= globalScale.y;
            rotatedVertex.z *= globalScale.z;

            // グローバル位置を適用
            Vector3 transformedVertex = rotatedVertex + globalTransform.Position;

            // 最小点と最大点を更新
            worldMin.x = std::min(worldMin.x, transformedVertex.x);
            worldMin.y = std::min(worldMin.y, transformedVertex.y);
            worldMin.z = std::min(worldMin.z, transformedVertex.z);

            worldMax.x = std::max(worldMax.x, transformedVertex.x);
            worldMax.y = std::max(worldMax.y, transformedVertex.y);
            worldMax.z = std::max(worldMax.z, transformedVertex.z);
        }

        return AABB(worldMin, worldMax);
    }

}
