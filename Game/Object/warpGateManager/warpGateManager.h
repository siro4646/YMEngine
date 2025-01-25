#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym
{
	class WarpGate;

	class WarpGateManager :public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		std::shared_ptr<WarpGate> warpGates1;
		std::shared_ptr<WarpGate> warpGates2;

	private:
	};
}