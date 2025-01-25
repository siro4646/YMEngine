#pragma once

#include "gameFrameWork/components/component.h"

namespace ym
{
	enum LightType
	{
		Directional,
		Point,
		Spot,
	};

	class Light :public Component
	{
	public:
		using Component::Component;

		void Init() override {}
		void FixedUpdate() override {}
		void Update() override {}
		void Draw() override {}
		void Uninit() override {}

	};
}