#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {
	class Material;
	class FBXMaterial;

	class OBJLoader;
	class FBXLoader;

	class DissolveMaterial;

	class PBRMaterial;

	class TornadoMaterial;

	//コピー用テンプレート
	class TestObject : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		OBJLoader *objLoader;
		FBXLoader *fbxLoader;
		float dissolve = 0.0f;

		std::vector<Mesh> meshes;
		std::vector<std::shared_ptr<Material>> materials;
		std::vector<std::shared_ptr<DissolveMaterial>> materials2;
		std::vector<std::shared_ptr<PBRMaterial>> materials3;
		std::vector<std::shared_ptr<TornadoMaterial>> materials4;

		//std::shared_ptr<FBXMaterial> fbxMaterial;


	};
}