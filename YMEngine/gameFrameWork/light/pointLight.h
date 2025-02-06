#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{ 
	class OBJLoader;
	struct Mesh;
	class DebugMaterial;

	struct PointLightData {
		DirectX::XMFLOAT3 position;   // ライトの位置 (12バイト)
		float padding1;               // パディング (4バイト)
		DirectX::XMFLOAT3 color;      // ライトの色 (12バイト)
		float padding2;               // パディング (4バイト)
		float intensity;              // ライトの強度 (4バイト)
		float radius;                 // ライトの影響範囲 (4バイト)
		float padding3[2];            // パディング (8バイト)
	};

	class PointLight : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;
	private:
		float randX;
		float randZ;
		float angle = (float)(rand() % 100);
		float r = 0;
		float g = 0;
		float b = 0;

		void SetLightData();
		PointLightData data;
		std::vector<std::shared_ptr<DebugMaterial>>materials;
		int factoryIndex{};
		OBJLoader *objLoader;
		std::vector<Mesh> meshes;

	};


}