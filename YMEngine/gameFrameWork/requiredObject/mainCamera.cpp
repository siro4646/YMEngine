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
		auto copy = std::make_shared<MainCamera>(*this); // �R�s�[�R���X�g���N�^���g�p
		// �q�I�u�W�F�N�g�͐[���R�s�[���s��
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
}

