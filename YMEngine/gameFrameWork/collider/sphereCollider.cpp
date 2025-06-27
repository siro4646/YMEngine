#include "collider.h"

#include "gameFrameWork/gameObject/gameObject.h"

#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"



namespace ym
{

	void SphereCollider::Init()
	{		
		radiusOffset = 0.0f;
		float radius = object->worldTransform.Scale.x * 0.5f + radiusOffset;
		transform.Position = GetCenter(); // 中心座標を初期化
		transform.Scale.x = radius * 2.0f;
		transform.Scale.y = radius * 2.0f;
		transform.Scale.z = radius * 2.0f; // YやZのスケールを無視して球体として扱う（直径でスケール）
		transform.Rotation = object->worldTransform.Rotation;

		Collider::Init();


	}
	void SphereCollider::Update()
	{
		transform.Position = GetCenter();
		float radius = GetScale().x + radiusOffset;
		// YやZのスケールを無視して球体として扱う（直径でスケール）
		transform.Scale.x = radius * 2.0f;
		transform.Scale.y = radius * 2.0f;
		transform.Scale.z = radius * 2.0f; // YやZのスケールを無視して球体として扱う（直径でスケール）
		transform.Rotation = object->worldTransform.Rotation;
	}
	void SphereCollider::FixedUpdate()
	{
		Collider::FixedUpdate();
	}
	bool SphereCollider::IsColliding(const Collider &other) const
	{
		return other.CollideWithSphere(*this);
	}
	bool SphereCollider::CollideWithSphere(const SphereCollider &sphere) const
	{

		float radiusA = GetScale().x;
		float radiusB = sphere.GetScale().x;

		Vector3 centerA = GetCenter();
		Vector3 centerB = sphere.GetCenter();

		float distSq = (centerA - centerB).LengthSquared();
		float radiusSum = radiusA + radiusB;

		return distSq <= radiusSum * radiusSum;
	}
	bool SphereCollider::CollideWithBox(const BoxCollider &box) const
	{
		auto center = GetCenter();
		auto boxScale = box.GetScale();
		Vector3 local = center - box.GetCenter();

		Vector3 closestPoint = Vector3::zero;
		auto axes = box.GetLocalAxes();

		for (int i = 0; i < 3; ++i) {
			float proj = local.Dot(axes[i]);
			proj = std::clamp(proj, -boxScale[i], boxScale[i]);
			closestPoint += proj * axes[i];
		}

		Vector3 diff = local - closestPoint;
		float distSq = diff.LengthSquared();

		float radius = GetScale().x;
		return distSq <= radius * radius;
	}

	// レイと球体との交差判定
	bool SphereCollider::Raycast(const Ray &ray, float &t, Vector3 &hitPoint) const {
		Vector3 center = GetCenter();               // 球体の中心
		float radius = GetScale().x;                // 半径（Xスケールから）

		Vector3 oc = ray.origin - center;           // 原点から球体中心までのベクトル
		float a = ray.direction.Dot(ray.direction); // レイの方向ベクトルの長さの2乗
		float b = 2.0f * oc.Dot(ray.direction);     // 2 * (oc・d)
		float c = oc.Dot(oc) - radius * radius;     // ocの長さの2乗 - 半径の2乗

		float discriminant = b * b - 4 * a * c;     // 判別式

		if (discriminant < 0)
			return false; // 解なし（交差しない）

		float sqrtD = sqrtf(discriminant);
		float t0 = (-b - sqrtD) / (2.0f * a);
		float t1 = (-b + sqrtD) / (2.0f * a);

		// 最も近い正のt値を選択
		t = (t0 > 0) ? t0 : ((t1 > 0) ? t1 : -1.0f);

		if (t > 0) {
			hitPoint = ray.origin + ray.direction * t;
			return true;
		}

		return false;
	}



	void SphereCollider::Draw()
	{
		// デバッグ用の描画処理（ワイヤフレームのスフィアなど）
		debug::DebugDraw::Instance().WireSphere(object->worldTransform, Vector3(1, 0, 0));
	}
	void SphereCollider::Uninit()
	{
		Collider::Uninit();
	}
	bool SphereCollider::IsCollidingAt(const Vector3 &futurePosition, const Collider &other)
	{
		Vector3 originalPosition = object->worldTransform.Position;
		SetCenter(futurePosition);
		bool result = IsColliding(other);
		SetCenter(originalPosition);
		return result;
	}
}