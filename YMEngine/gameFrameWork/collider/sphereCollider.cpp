#include "collider.h"

#include "gameFrameWork/gameObject/gameObject.h"

#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"

namespace ym
{

	void SphereCollider::Init()
	{
		const wchar_t *modelFile = L"asset/model/sphere_smooth.obj";
		//material->Init();

		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile,
			flag
		};

		auto objLoader = object->AddComponent<OBJLoader>().get();
		auto meshes = objLoader->Load(importSetting);
		int count = 0;
		for (auto &mesh : meshes)
		{
			auto debugMaterial = std::make_shared<DebugMaterial>();
			//debugMaterial->SetMainTex("asset/texture/white.dds");
			objLoader->SetMaterial(debugMaterial, count);
			count++;
		}		
		Collider::Init();




	}
	void SphereCollider::Update()
	{
		// トランスフォームからスフィアの情報を更新
		auto transform = object->worldTransform;		
		this->radius = transform.Scale.x * 0.5f; // 半径
	}
	void SphereCollider::FixedUpdate()
	{
		sceneOctree->Remove(object);
		sceneOctree->Insert(object);
	}
	bool SphereCollider::IsColliding(const Collider &other) const
	{
		return other.CollideWithSphere(*this);
	}
	bool SphereCollider::CollideWithSphere(const SphereCollider &sphere) const
	{

		auto center = GetCenter();
		auto sphereCenter = sphere.GetCenter();

		float distanceSquared = (center - sphereCenter).LengthSquared();
		float radiusSum = radius + sphere.radius;

		// 中心間の距離が半径の合計以下なら衝突
		return distanceSquared <= radiusSum * radiusSum;
	}
	bool SphereCollider::CollideWithBox(const BoxCollider &box) const
	{
		auto center = GetCenter();
		auto boxScale = box.GetScale();



		// スフィアの中心をボックスのローカル空間に変換
		Vector3 sphereCenterInLocal = center - box.GetCenter();

		// ローカル空間で最近接点を求める
		Vector3 closestPoint = Vector3::zero;
		auto axes = box.GetLocalAxes(); // OBBのローカル軸 (X, Y, Z)

		for (int i = 0; i < 3; ++i) {
			float projection = sphereCenterInLocal.Dot(axes[i]);
			// ボックスのスケール内に制限
			projection = std::clamp(projection, -boxScale[i], boxScale[i]);
			// 最近接点をローカル空間で求める
			closestPoint += projection * axes[i];
		}

		// スフィア中心と最近接点間の距離の平方
		Vector3 distanceVector = sphereCenterInLocal - closestPoint;
		float distanceSquared = distanceVector.LengthSquared();

		

		// 衝突判定（距離の平方で比較）
		return distanceSquared <= (radius * radius);
	}


	void SphereCollider::Draw()
	{
		// デバッグ用の描画処理（ワイヤフレームのスフィアなど）
	}
	void SphereCollider::Uninit()
	{
		Collider::Uninit();
	}
}