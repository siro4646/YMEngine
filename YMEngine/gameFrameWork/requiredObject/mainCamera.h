#pragma once
#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {

	class Camera;

	class MainCamera : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;
	private:

		void UpdateCamera();

		float distance_ = 100.0f;

		Camera *camera_;
	};
}