#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"
#include "playerController.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"
#include "utility/inputSystem/mouse/mouseInput.h"
#include "gameFrameWork/components/rigidBody/rigidBody.h"

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
	void PlayerController::Init()
	{
		rb = object->GetComponent<Rigidbody>().get();
		
	}
	void PlayerController::FixedUpdate()
	{
	}
	void PlayerController::Update()
	{
		auto &kb = KeyboardInput::GetInstance();
		auto &mouse = MouseInput::GetInstance();
		const float speed = 10;

		if (kb.GetKey("W"))
		{
			auto forward = object->localTransform.GetForward();
			rb->AddForce(forward*speed);
		}
		if (kb.GetKey("S"))
		{
			auto forward = object->localTransform.GetForward();
			rb->AddForce(forward * -speed);
		}

		if (kb.GetKey("A"))
		{
			auto right = object->localTransform.GetRight();
			rb->AddForce(right * -speed);
		}

		if (kb.GetKey("D"))
		{
			auto right = object->localTransform.GetRight();
			rb->AddForce(right * speed);
		}

	}
	void PlayerController::Draw()
	{
	}
	void PlayerController::Uninit()
	{
	}
}
