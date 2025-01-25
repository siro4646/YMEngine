#include "sceneRenderRegistrar.h"

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
	void SceneRenderRegistrar::Init()
	{
	}
	void SceneRenderRegistrar::Uninit()
	{
		renderObjects_.clear();
	}
	void SceneRenderRegistrar::Draw()
	{
		for (auto object : renderObjects_)
		{
			object->Draw();
		}
	}
}
