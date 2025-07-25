#include "collider/collider.h"
#include "gameFrameWork/gameObject/gameObject.h"

#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"


namespace ym
{
	

	void BoxCollider::Init()
	{
		const wchar_t *modelFile = L"asset/model/cube.obj";
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
		
		//オクツリーに追加
		//sceneOctree->Insert(object);
	}

	void BoxCollider::Update()
	{
		
	}

	void BoxCollider::FixedUpdate()
	{
		//もし動作が重かったら考える
		// オクツリーから削除して再登録
		sceneOctree->Remove(object);
		sceneOctree->Insert(object);
	}

	void BoxCollider::Draw()
	{

	}

	void BoxCollider::Uninit()
	{
		Collider::Uninit();

	}

	/**
	 * ローカル座標系の各軸を計算する
	 * @return 各軸の方向ベクトルを格納した配列
	 */
	std::array<Vector3, 3> BoxCollider::GetLocalAxes() const
	{
		auto rotation = GetRotation(); // 回転角度
		// 度をラジアンに変換
		float radX = rotation.x * M_PI / 180.0f;
		float radY = rotation.y * M_PI / 180.0f;
		float radZ = rotation.z * M_PI / 180.0f;

		float sinX = sinf(radX), cosX = cosf(radX);
		float sinY = sinf(radY), cosY = cosf(radY);
		float sinZ = sinf(radZ), cosZ = cosf(radZ);

		// ローカルX軸（1,0,0を回転させた結果）
		Vector3 localX(
			cosY * cosZ,
			cosY * sinZ,
			-sinY
		);

		// ローカルY軸（0,1,0を回転させた結果）
		Vector3 localY(
			sinX * sinY * cosZ - cosX * sinZ,
			sinX * sinY * sinZ + cosX * cosZ,
			sinX * cosY
		);

		// ローカルZ軸（0,0,1を回転させた結果）
		Vector3 localZ(
			cosX * sinY * cosZ + sinX * sinZ,
			cosX * sinY * sinZ - sinX * cosZ,
			cosX * cosY
		);

		return { localX, localY, localZ };
	}

	bool BoxCollider::IsColliding(const Collider &other) const
	{
		return other.CollideWithBox(*this);		
	}
	bool BoxCollider::CollideWithBox(const BoxCollider &box) const
	{
		return OBBIntersects(*this, box); // OBB同士の衝突判定
	}
	bool BoxCollider::CollideWithSphere(const SphereCollider &sphere) const
	{
		return sphere.CollideWithBox(*this);
	}
	/**
 * OBB同士の衝突判定（分離軸定理を使用）
 */
	bool BoxCollider::OBBIntersects(const BoxCollider &a, const BoxCollider &b) const {
		// OBBの各軸（ローカル座標系）を取得
		auto aAxes = a.GetLocalAxes(); // Aのローカル軸 (X, Y, Z)
		auto aCenter = a.GetCenter(); // Aの中心座標
		auto aScale = a.GetScale(); // Aのスケール
		auto bAxes = b.GetLocalAxes(); // Bのローカル軸 (X, Y, Z)
		auto bCenter = b.GetCenter(); // Bの中心座標
		auto bScale = b.GetScale(); // Bのスケール


		// 中心間のベクトル
		Vector3 T = bCenter - aCenter;

		// Aのローカル空間に変換
		T = Vector3(T.Dot(aAxes[0]), T.Dot(aAxes[1]), T.Dot(aAxes[2]));

		// Aの軸を分離軸として判定
		for (int i = 0; i < 3; ++i) {
			float RA = aScale[i]; // Aの軸方向の半径
			float RB = bScale.x * std::abs(bAxes[0].Dot(aAxes[i])) +
				bScale.y * std::abs(bAxes[1].Dot(aAxes[i])) +
				bScale.z * std::abs(bAxes[2].Dot(aAxes[i]));

			if (std::abs(T[i]) > RA + RB) return false; // 分離軸が見つかった
		}

		// Bの軸を分離軸として判定
		for (int i = 0; i < 3; ++i) {
			float RA = aScale.x * std::abs(aAxes[0].Dot(bAxes[i])) +
				aScale.y * std::abs(aAxes[1].Dot(bAxes[i])) +
				aScale.z * std::abs(aAxes[2].Dot(bAxes[i]));
			float RB = bScale[i]; // Bの軸方向の半径

			if (std::abs(T.Dot(bAxes[i])) > RA + RB) return false; // 分離軸が見つかった
		}
		// クロス軸による分離軸を判定
		for (int i = 0; i < 3; ++i) {        // Aの軸
			for (int j = 0; j < 3; ++j) {    // Bの軸
				// クロス軸 = aAxes[i] × bAxes[j]
				Vector3 crossAxis = aAxes[i].Cross(bAxes[j]);
				if (crossAxis.LengthSquared() < 1e-6f) continue; // 長さがゼロの場合はスキップ

				// AとBをクロス軸に投影
				float RA = aScale.x * std::abs(aAxes[0].Dot(crossAxis)) +
					aScale.y * std::abs(aAxes[1].Dot(crossAxis)) +
					aScale.z * std::abs(aAxes[2].Dot(crossAxis));

				float RB = bScale.x * std::abs(bAxes[0].Dot(crossAxis)) +
					bScale.y * std::abs(bAxes[1].Dot(crossAxis)) +
					bScale.z * std::abs(bAxes[2].Dot(crossAxis));

				float distance = std::abs(T.Dot(crossAxis));

				if (distance > RA + RB) return false; // 分離軸が見つかった
			}
		}

		return true; // 分離軸が見つからなかった場合、衝突している
	}

	Vector3 BoxCollider::GetClosestPoint(const Vector3 &point)const
	{
		Vector3 center = GetCenter(); // ボックスの中心座標
		Vector3 localPoint = point - center; // ボックスのローカル座標系に変換
		Vector3 closestPoint = Vector3::zero;
		Vector3 scale = GetScale(); // ボックスのスケール

		auto axes = GetLocalAxes(); // ローカル軸
		for (int i = 0; i < 3; ++i) {
			float projection = localPoint.Dot(axes[i]);
			projection = std::max(-scale[i], std::min(projection, scale[i]));
			closestPoint += projection * axes[i];
		}

		return closestPoint + center; // ワールド座標に戻す
	}

}