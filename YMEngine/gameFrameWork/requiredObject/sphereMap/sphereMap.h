#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
	class OBJLoader;
	struct Mesh;	
	class PBRMaterial;
	class OBJMaterial;

	class SphereMap : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;
	private:
		std::vector<std::shared_ptr<OBJMaterial>>materials;
		OBJLoader *objLoader;
		std::vector<Mesh> meshes;

	};


}