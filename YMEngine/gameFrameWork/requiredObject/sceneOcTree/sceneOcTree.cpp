#include "sceneOcTree.h"

#include "gameFrameWork/gameObject/gameObject.h"

#include "ocTree/ocTree.h"


namespace ym
{
	void SceneOcTree::Init()
	{
		name = "SceneOcTree";

		AABB sceneBounds(Vector3(
			SCENE_MIN_SIZE,
			SCENE_MIN_SIZE,
			SCENE_MIN_SIZE),
			Vector3(
				SCENE_MAX_SIZE,
				SCENE_MAX_SIZE,
				SCENE_MAX_SIZE));

		sceneOctree = std::make_shared<Octree>(sceneBounds,10,10);
	}

	void SceneOcTree::FixedUpdate()
	{

		Object::FixedUpdate();

	}

	void SceneOcTree::Update()
	{
		Object::Update();
	}

	void SceneOcTree::Draw()
	{
		Object::Draw();
	}

	void SceneOcTree::Uninit()
	{
	
		Object::Uninit();
	}
	std::shared_ptr<Object> SceneOcTree::Clone()
	{
		auto copy = std::make_shared<SceneOcTree>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
}