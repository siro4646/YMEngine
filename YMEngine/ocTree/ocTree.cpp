#include "ocTree.h"
#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"

namespace ym
{
    AABB GetAABB(Object *object) {
        auto &local = object->localTransform;
        auto &world = object->worldTransform;

        std::vector<Vector3> localVertices = {
            {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
            {-0.5f,  0.5f, -0.5f}, {0.5f,  0.5f, -0.5f},
            {-0.5f, -0.5f,  0.5f}, {0.5f, -0.5f,  0.5f},
            {-0.5f,  0.5f,  0.5f}, {0.5f,  0.5f,  0.5f}
        };

        Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        XMMATRIX rot = world.GetMatrixRotation();
        Vector3 scale = world.Scale;

        for (const auto &vertex : localVertices) {
            Vector3 v(
                vertex.x * local.Scale.x,
                vertex.y * local.Scale.y,
                vertex.z * local.Scale.z
            );
            auto exVec = v.ToXMFLOAT3();
            XMVECTOR vec = XMVector3Transform(XMLoadFloat3(&exVec), rot);
            Vector3 rotated(
                XMVectorGetX(vec) * scale.x,
                XMVectorGetY(vec) * scale.y,
                XMVectorGetZ(vec) * scale.z
            );

            Vector3 transformed = rotated + world.Position;

            min = Vector3::Min(min, transformed);
            max = Vector3::Max(max, transformed);
        }

        return AABB(min, max);
    }
}
