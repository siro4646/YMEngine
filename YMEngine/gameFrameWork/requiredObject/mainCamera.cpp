#include "mainCamera.h"
#include "camera/cameraManager.h"
#include "camera/camera.h"

namespace ym
{
	void MainCamera::Init()
	{
		camera_ = CameraManager::Instance().CreateCamera("mainCamera");

	}

	void MainCamera::FixedUpdate()
	{
	}

	void MainCamera::Update()
	{
		camera_->UpdateViewMatrix();
		camera_->UpdateProjectionMatrix();
		camera_->UpdateShaderBuffer();
	}

	void MainCamera::Draw()
	{
	}

	void MainCamera::Uninit()
	{
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
}

