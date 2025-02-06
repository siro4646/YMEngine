#pragma once

#include"scene/baseScene.h"

namespace ym
{
	class Renderer;
	class Object;

	class MainCamera;
	class SphereMap;

	class TitleScene : public BaseScene
	{
	public:
		TitleScene() {}
		~TitleScene()
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