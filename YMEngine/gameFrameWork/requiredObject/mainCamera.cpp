#include "mainCamera.h"
#include "camera/cameraManager.h"
#include "camera/camera.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

namespace ym
{
	void MainCamera::Init()
	{
		name = "MainCamera";
		camera_ = CameraManager::Instance().CreateCamera("mainCamera");

	}

	void MainCamera::FixedUpdate()
	{
	}

	void MainCamera::Update()
	{		
		auto &input = KeyboardInput::GetInstance();
		if (input.GetKey("W"))
		{
			auto forward = globalTransform.GetForward();
			globalTransform.Position += 0.05f*forward;
		}
		if (input.GetKey("S"))
		{
			//globalTransform.Position.z -= 0.05f;
			auto forward = globalTransform.GetForward();
			globalTransform.Position -= 0.05f * forward;

		}
		if (input.GetKey("A"))
		{
			auto right = globalTransform.GetRight();

			globalTransform.Position-= 0.05f*right;
		}
		if (input.GetKey("D"))
		{
			auto right = globalTransform.GetRight();

			globalTransform.Position += 0.05f * right;
		}
		if (input.GetKey("Q"))
		{
			globalTransform.Position.y += 0.05f;
		}
		if (input.GetKey("E"))
		{
			globalTransform.Position.y -= 0.05f;
		}
		//サイズ変える
		if (input.GetKey("T"))
		{
			globalTransform.Rotation.y -= 1.0f;
		}
		if (input.GetKey("Y"))
		{
			globalTransform.Rotation.y += 1.0f;

		}
		if (input.GetKey("0"))
		{
			globalTransform.Position = Vector3::zero;
			globalTransform.Rotation = Vector3::zero;
		}

		UpdateCamera();
		camera_->UpdateViewMatrix();
		camera_->UpdateProjectionMatrix();
		camera_->UpdateShaderBuffer();

		Object::Update();
	}

	void MainCamera::Draw()
	{
	}

	void MainCamera::Uninit()
	{
		//camera_->Uninit();
		//camera_ = nullptr;
	}
	std::shared_ptr<Object> MainCamera::Clone()
	{
		auto copy = std::make_shared<MainCamera>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
	void MainCamera::UpdateCamera()
	{
		camera_->SetEye(globalTransform.Position);
		camera_->SetTarget(globalTransform.Position + globalTransform.GetForward()* distance_);
		camera_->SetDistance(distance_);
		camera_->SetUp(globalTransform.GetUp());

	}
}

