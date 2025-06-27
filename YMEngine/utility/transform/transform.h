#pragma once

#include "utility/vector/vector2.h"
#include "utility/vector/vector3.h"
#include "utility/quaternion/quaternion.h"


namespace ym {
	class Object;
	/*class Vector2;
	class Vector3;*/

	class Transform2D
	{
	public:
		//���W
		Vector2 Position;
		//�T�C�Y
		Vector2 Scale;
		//Z����̉�]
		float Rotation;
		Transform2D() : Position(0, 0), Scale(1.f, 1.f), Rotation(0) {}
		Transform2D(Vector2 pos, Vector2 scale, float rot) : Position(pos), Scale(scale), Rotation(rot) {}
		~Transform2D() {}
	};
	class Transform
	{
	public:
		//���W
		Vector3 Position;
		//�T�C�Y
		Vector3 Scale;
		//����
		Vector3 Rotation;
		//�N�H�[�^�j�I��
		Quaternion quaternion;

		Object *gameObject = nullptr;

		Transform() : Position(0, 0, 0), Scale(1.f, 1.f, 1.f), Rotation(0, 0, 0) {}
		Transform(Vector3 pos, Vector3 scale, Vector3 rot) : Position(pos), Scale(scale), Rotation(rot) {}

		static Transform FromMatrix(const XMMATRIX &matrix)
		{
			Transform transform;

			XMVECTOR scaleVec, rotQuat, translation;
			if (XMMatrixDecompose(&scaleVec, &rotQuat, &translation, matrix))
			{
				// �ʒu
				transform.Position = Vector3(
					XMVectorGetX(translation),
					XMVectorGetY(translation),
					XMVectorGetZ(translation)
				);

				// �X�P�[��
				transform.Scale = Vector3(
					XMVectorGetX(scaleVec),
					XMVectorGetY(scaleVec),
					XMVectorGetZ(scaleVec)
				);

				// ��]�i�I�C���[�p�ɕϊ��j
				XMFLOAT4 q;
				XMStoreFloat4(&q, rotQuat);

				float ysqr = q.y * q.y;

				// Pitch�iX���j
				float t0 = +2.0f * (q.w * q.x + q.y * q.z);
				float t1 = +1.0f - 2.0f * (q.x * q.x + ysqr);
				float pitch = atan2(t0, t1);

				// Yaw�iY���j
				float t2 = +2.0f * (q.w * q.y - q.z * q.x);
				t2 = std::clamp(t2, -1.0f, 1.0f);
				float yaw = asin(t2);

				// Roll�iZ���j
				float t3 = +2.0f * (q.w * q.z + q.x * q.y);
				float t4 = +1.0f - 2.0f * (ysqr + q.z * q.z);
				float roll = atan2(t3, t4);

				transform.Rotation = Vector3(
					XMConvertToDegrees(pitch),
					XMConvertToDegrees(yaw),
					XMConvertToDegrees(roll)
				);
			}

			return transform;
		}

		inline void SetObject(Object *obj) {
			gameObject = obj;
		}
		inline Object *GetOwner() {
			return gameObject;
		}


		inline XMMATRIX GetMatrixRotation()
		{
			XMMATRIX x = XMMatrixRotationX(XMConvertToRadians(Rotation.x));
			XMMATRIX y = XMMatrixRotationY(XMConvertToRadians(Rotation.y));
			XMMATRIX z = XMMatrixRotationZ(XMConvertToRadians(Rotation.z));
			return x * y * z;
		}
		inline XMMATRIX GetMatrixPosition()
		{
			return XMMatrixTranslation(Position.x, Position.y, Position.z);
		}
		inline XMMATRIX GetMatrixScale()
		{
			return XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		}
		inline XMMATRIX GetMatrix() {
			// �X�P�[�� �� ��] �� ���s�ړ�
			return GetMatrixScale() * GetMatrixRotation() * GetMatrixPosition();
		}

		inline Vector3 GetForward()
		{
			XMMATRIX m = GetMatrixRotation();
			XMVECTOR v = XMVectorSet(0, 0, 1, 0);
			v = XMVector3Transform(v, m);
			return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
		}
		inline Vector3 GetRight()
		{
			XMMATRIX m = GetMatrixRotation();
			XMVECTOR v = XMVectorSet(1, 0, 0, 0);
			v = XMVector3Transform(v, m);
			return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
		}
		inline Vector3 GetUp()
		{
			XMMATRIX m = GetMatrixRotation();
			XMVECTOR v = XMVectorSet(0, 1, 0, 0);
			v = XMVector3Transform(v, m);
			return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
		}
		inline Transform GetWorldTransform(Transform &parentTransform) {
			// ���[�J���Ɛe�̍s����v�Z
			XMMATRIX localMatrix = this->GetMatrix();
			XMMATRIX parentWorldMatrix = parentTransform.GetMatrix();
			XMMATRIX worldMatrix = localMatrix * parentWorldMatrix;

			// ���[���hTransform
			ym::Transform worldTransform;

			// �ʒu�̒��o
			XMVECTOR position = worldMatrix.r[3];
			worldTransform.Position = ym::Vector3(
				XMVectorGetX(position),
				XMVectorGetY(position),
				XMVectorGetZ(position)
			);

			// �X�P�[���̒��o
			XMVECTOR scaleX = XMVector3Length(worldMatrix.r[0]);
			XMVECTOR scaleY = XMVector3Length(worldMatrix.r[1]);
			XMVECTOR scaleZ = XMVector3Length(worldMatrix.r[2]);
			worldTransform.Scale = ym::Vector3(
				XMVectorGetX(scaleX),
				XMVectorGetX(scaleY),
				XMVectorGetX(scaleZ)
			);

			// ��]�̒��o�i�X�P�[���������j
			XMVECTOR normalizedX = XMVector3Normalize(worldMatrix.r[0]);
			XMVECTOR normalizedY = XMVector3Normalize(worldMatrix.r[1]);
			XMVECTOR normalizedZ = XMVector3Normalize(worldMatrix.r[2]);

			// �s�b�`�iX����]�j
			float pitch = atan2(XMVectorGetZ(normalizedY), XMVectorGetZ(normalizedZ));
			// ���[�iY����]�j
			float yaw = atan2(-XMVectorGetZ(normalizedX), XMVectorGetX(XMVector3Length(XMVectorSet(XMVectorGetX(normalizedX), XMVectorGetY(normalizedX), 0, 0))));
			// ���[���iZ����]�j
			float roll = atan2(XMVectorGetY(normalizedX), XMVectorGetX(normalizedX));

			worldTransform.Rotation = ym::Vector3(
				XMConvertToDegrees(pitch),
				XMConvertToDegrees(yaw),
				XMConvertToDegrees(roll)
			);

			return worldTransform;
		}


		void Rotate(Vector3 rot)
		{
		//	auto delta = Application::Instance()->GetDeltaTime();
			Rotation += rot;
		}

	};
}