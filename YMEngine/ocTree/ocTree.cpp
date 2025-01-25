#include "ocTree.h"
#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"

namespace ym
{
    AABB GetAABB(Object *object) {
        // ���[�J����Ԃ�8�̒��_���
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

        // ���[���h��Ԃł̍ŏ��_�E�ő�_�̏�����
        Vector3 worldMin(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 worldMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        // �O���[�o���X�P�[���Ɖ�]�s����擾
        XMMATRIX rotationMatrix = globalTransform.GetMatrixRotation();
        Vector3 globalScale = globalTransform.Scale;

        // �e���_�����[���h��Ԃɕϊ�
        for (const auto &vertex : localVertices) {
            // ���[�J���X�P�[����K�p
            Vector3 scaledVertex = Vector3(vertex.x * localTransform.Scale.x,
                vertex.y * localTransform.Scale.y,
                vertex.z * localTransform.Scale.z);

            // ��]��K�p
            auto toFloat3 = scaledVertex.ToXMFLOAT3();
            XMVECTOR v = XMVector3Transform(XMLoadFloat3(&toFloat3), rotationMatrix);
            Vector3 rotatedVertex(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);

            // �O���[�o���X�P�[����K�p
            rotatedVertex.x *= globalScale.x;
            rotatedVertex.y *= globalScale.y;
            rotatedVertex.z *= globalScale.z;

            // �O���[�o���ʒu��K�p
            Vector3 transformedVertex = rotatedVertex + globalTransform.Position;

            // �ŏ��_�ƍő�_���X�V
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
