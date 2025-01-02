#pragma once

#include "utility/vector/vector2.h"
#include "utility/vector/vector3.h"

namespace ym {
	/*class Vector2;
	class Vector3;*/

	class Transform2D
	{
	public:
		//座標
		Vector2 Position;
		//サイズ
		Vector2 Scale;
		//Z軸上の回転
		float Rotation;
		Transform2D() : Position(0, 0), Scale(1.f, 1.f), Rotation(0) {}
		Transform2D(Vector2 pos, Vector2 scale, float rot) : Position(pos), Scale(scale), Rotation(rot) {}
		~Transform2D() {}
	};
	class Transform
	{
	public:
		//座標
		Vector3 Position;
		//サイズ
		Vector3 Scale;
		//向き
		Vector3 Rotation;
		Transform() : Position(0, 0, 0), Scale(1.f, 1.f, 1.f), Rotation(0, 0, 0) {}
		Transform(Vector3 pos, Vector3 scale, Vector3 rot) : Position(pos), Scale(scale), Rotation(rot) {}

		XMMATRIX GetMatrixRotation()
		{
			XMMATRIX x = XMMatrixRotationX(XMConvertToRadians(Rotation.x));
			XMMATRIX y = XMMatrixRotationY(XMConvertToRadians(Rotation.y));
			XMMATRIX z = XMMatrixRotationZ(XMConvertToRadians(Rotation.z));
			return x * y * z;
		}
		XMMATRIX GetMatrixPosition()
		{
			return XMMatrixTranslation(Position.x, Position.y, Position.z);
		}
		XMMATRIX GetMatrixScale()
		{
			return XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		}
		XMMATRIX GetMatrix()
		{
			//return GetMatrixPosition() * GetMatrixRotation() * GetMatrixScale();
			return GetMatrixScale() * GetMatrixRotation() * GetMatrixPosition();
		}
		Vector3 GetForward()
		{
			XMMATRIX m = GetMatrixRotation();
			XMVECTOR v = XMVectorSet(0, 0, 1, 0);
			v = XMVector3Transform(v, m);
			return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
		}
		Vector3 GetRight()
		{
			XMMATRIX m = GetMatrixRotation();
			XMVECTOR v = XMVectorSet(1, 0, 0, 0);
			v = XMVector3Transform(v, m);
			return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
		}
		Vector3 GetUp()
		{
			XMMATRIX m = GetMatrixRotation();
			XMVECTOR v = XMVectorSet(0, 1, 0, 0);
			v = XMVector3Transform(v, m);
			return Vector3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
		}

	};
}