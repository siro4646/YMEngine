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

	class Camera;

	class Rigidbody;

	class MainCamera;

	class RenderTexture;


	//コピー用テンプレート
	class Player : public Object
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

		Object *mainCamera;

		float dissolve = 0.0f;

		std::vector<Mesh> meshes;
		std::vector<std::shared_ptr<PBRMaterial>> materials3;
		//std::shared_ptr<FBXMaterial> fbxMaterial;

		Camera *mainCamera_;
		Camera *subCamera_;

		std::shared_ptr<RenderTexture> renderTexture;

		Rigidbody *rb;

	};
}