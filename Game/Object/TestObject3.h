#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {
	class Material;
	class FBXMaterial;

	class OBJLoader;
	class FBXLoader;

	class Camera;

	class PBRMaterial;


	//コピー用テンプレート
	class TestObject3 : public Object
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
		Camera *camera;

		std::vector<Mesh> meshes;
		std::vector<std::shared_ptr<Material>> materials;
		std::vector<std::shared_ptr<Material>> materials2;
		std::vector<std::shared_ptr<PBRMaterial>> materials3;

		//std::shared_ptr<FBXMaterial> fbxMaterial;
	private:
		

	};
}