#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
	#define SCENE_MIN_SIZE (-1000)
#define SCENE_MAX_SIZE (1000)
	class Octree;
	struct AABB;
	class SceneOcTree : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		inline Octree *GetSceneOctree() { return sceneOctree.get(); }
	private:
		std::shared_ptr<Octree> sceneOctree;
	};


}