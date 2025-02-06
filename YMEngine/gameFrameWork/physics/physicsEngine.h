#pragma once

//#include "gameFrameWork/components/rigidBody/rigidBody.h"

namespace ym
{
	class Rigidbody;
	class PhysicsEngine
	{


	public:
		static PhysicsEngine *Instance()
		{
			static PhysicsEngine instance;
			return &instance;
		}

		void Init();
		void Uninit();
		void Update();
		void FixedUpdate();

		void RegisterRigidbody(Rigidbody *rigidbody);



	private:

		std::vector<Rigidbody *> rigidbodies_;

	};
}