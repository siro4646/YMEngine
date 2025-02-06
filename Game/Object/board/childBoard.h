#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {

	class OBJLoader;
	class Material;
	class Mesh;
	
	enum class KomaType;

	//コピー用テンプレート
	class ChildBoard : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		void PutKoma(KomaType komaType);
		void FlipKoma(KomaType komaType,float flipTime);

		bool IsEmpty() { return _childs.empty(); }

		KomaType GetKomaType() { return _komaType; }

		void SetTexture(string texturePath);

	private:
		KomaType _komaType;
		OBJLoader *objLoader{};

		std::vector<Mesh> meshes;

	};
}