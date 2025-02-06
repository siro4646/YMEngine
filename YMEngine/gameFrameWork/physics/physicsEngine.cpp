#include "physicsEngine.h"
#include "gameFrameWork/components/rigidBody/rigidBody.h"


void ym::PhysicsEngine::Init()
{

}

void ym::PhysicsEngine::Uninit()
{

}

void ym::PhysicsEngine::Update()
{


}

void ym::PhysicsEngine::FixedUpdate()
{

}

void ym::PhysicsEngine::RegisterRigidbody(Rigidbody *rigidbody)
{
	//Šù‚É“o˜^‚³‚ê‚Ä‚¢‚é‚©‚Ç‚¤‚©‚ðŠm”F
	for (auto rb : rigidbodies_)
	{
		if (rb == rigidbody)
		{
			return;
		}
	}

	rigidbodies_.push_back(rigidbody);
}
