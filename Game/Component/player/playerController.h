#pragma once

#include "gameFrameWork/components/component.h"

namespace ym
{
	class Component;
	class Rigidbody;
	
	class PlayerController : public Component
	{
	public:
		using Component::Component;

		void Init()override;
		void FixedUpdate() override;
		void Update()override;
		void Draw()override;

		void Uninit()override;

	private:
		Rigidbody *rb{};

	};

}