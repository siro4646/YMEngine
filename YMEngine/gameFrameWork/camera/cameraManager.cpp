
#include "cameraManager.h"

#include "camera.h"

namespace ym
{
	Camera* CameraManager::CreateCamera(const std::string &name)
	{
		if (cameras_.find(name) != cameras_.end())
		{
			return cameras_[name].get();
		}
		auto pCamera = std::make_shared<Camera>();
		pCamera->name_ = name;
		cameras_[name] = pCamera;
		// ���C���J�������ݒ肳��Ă��Ȃ��ꍇ�͐ݒ�
		if (pMainCamera_ == nullptr)
		{
			pMainCamera_ = pCamera;
		}
		return pCamera.get();

	}
	Camera* CameraManager::GetCamera(const std::string &name) const
	{
		auto it = cameras_.find(name);
		if (it != cameras_.end())
		{
			return it->second.get();
		}
		// �J������������Ȃ��ꍇ��nullptr��Ԃ�	
		return nullptr;
	}
	void CameraManager::SetMainCamera(const std::string &name)
	{
		auto it = cameras_.find(name);
		if (it != cameras_.end())
		{
			pMainCamera_ = it->second;
		}
	}
}
