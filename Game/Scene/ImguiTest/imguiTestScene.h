#pragma once

#include"scene/baseScene.h"

namespace ym
{
	class Renderer;
	class Object;

	class MainCamera;
	class SphereMap;

	class ImguiTestScene : public BaseScene
	{
	public:
		ImguiTestScene() {}
		~ImguiTestScene()
		{
			UnInit();
		}

		void Init() override;
		void UnInit() override;
		void FixedUpdate() override;
		void Update() override;
		void Draw() override;

	private:
	Renderer *_renderer = nullptr;
		std::shared_ptr<Object> _mainCamera;

	};
}