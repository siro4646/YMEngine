#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {
	class Material;
	class FBXMaterial;

	class OBJLoader;

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
		std::vector<Mesh> meshes;
		std::vector<std::shared_ptr<Material>> materials;
		//std::shared_ptr<FBXMaterial> fbxMaterial;


	};
}