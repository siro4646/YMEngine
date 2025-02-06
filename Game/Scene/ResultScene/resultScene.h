#pragma once

#include"scene/baseScene.h"

namespace ym
{
	class Renderer;
	class Object;

	class MainCamera;
	class SphereMap;

	class ResultScene : public BaseScene
	{
	public:
		ResultScene() {}
		~ResultScene()
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

		std::shared_ptr<Object> _testObject;
		std::shared_ptr<Object> _testObject2;
		std::shared_ptr<Object> pointLight;
		std::shared_ptr<Object> _mainCamera;

	};
}