#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{ 
	class OBJLoader;
	struct Mesh;
	class DebugMaterial;

	struct PointLightData {
		DirectX::XMFLOAT3 position;   // ���C�g�̈ʒu (12�o�C�g)
		float padding1;               // �p�f�B���O (4�o�C�g)
		DirectX::XMFLOAT3 color;      // ���C�g�̐F (12�o�C�g)
		float padding2;               // �p�f�B���O (4�o�C�g)
		float intensity;              // ���C�g�̋��x (4�o�C�g)
		float radius;                 // ���C�g�̉e���͈� (4�o�C�g)
		float padding3[2];            // �p�f�B���O (8�o�C�g)
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