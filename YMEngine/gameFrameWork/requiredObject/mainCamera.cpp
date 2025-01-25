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
		Object::FixedUpdate();
	}

	void MainCamera::Update()
	{		
		auto &input = KeyboardInput::GetInstance();

		//親がいないなら
		if (!_parent)
		{
			if (input.GetKey("W"))
			{
				auto forward = localTransform.GetForward();
				localTransform.Position += 0.05f * forward;
			}
			if (input.GetKey("S"))
			{
				//localTransform.Position.z -= 0.05f;
				auto forward = localTransform.GetForward();
				localTransform.Position -= 0.05f * forward;

			}
			if (input.GetKey("A"))
			{
				auto right = localTransform.GetRight();

				localTransform.Position -= 0.05f * right;
			}
			if (input.GetKey("D"))
			{
				auto right = localTransform.GetRight();

				localTransform.Position += 0.05f * right;
			}
			if (input.GetKey("Q"))
			{
				localTransform.Position.y += 0.05f;
			}
			if (input.GetKey("E"))
			{
				localTransform.Position.y -= 0.05f;
			}
			//サイズ変える
			if (input.GetKey("T"))
			{
				localTransform.Rotation.y -= 1.0f;
			}
			if (input.GetKey("Y"))
			{
				localTransform.Rotation.y += 1.0f;

			}
			if (input.GetKey("0"))
			{
				localTransform.Position = Vector3::zero;
				//localTransform.Rotation = Vector3::zero;
			}
		}

		Object::Update();
		UpdateCamera();
		camera_->UpdateViewMatrix();
		camera_->UpdateProjectionMatrix();
		camera_->UpdateShaderBuffer();

		if (input.GetKeyDown("F"))
		{
			ym::ConsoleLog("Position:%f %f %f\n", worldTransform.Position.x, worldTransform.Position.y, worldTransform.Position.z);
			ym::ConsoleLog("Rotation:%f %f %f\n", worldTransform.Rotation.x, worldTransform.Rotation.y, worldTransform.Rotation.z);
			ym::ConsoleLog("Scale:%f %f %f\n", worldTransform.Scale.x, worldTransform.Scale.y, worldTransform.Scale.z);

			//camera_->SetProjectionType(ProjectionType::Perspective);
		}

	}

	void MainCamera::Draw()
	{
		Object::Draw();
	}

	void MainCamera::Uninit()
	{
		Object::Uninit();
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
		camera_->SetEye(worldTransform.Position);
		camera_->SetTarget(worldTransform.Position + worldTransform.GetForward()* distance_);
		camera_->SetDistance(distance_);
		camera_->SetUp(worldTransform.GetUp());

	}
}

