#pragma once

namespace ym 
{
	class Camera;

	class CameraManager
	{
    public:
        // �V���O���g���C���X�^���X�̎擾
        static CameraManager &Instance() 
        {
            static CameraManager instance;
            return instance;
        }

        // �J�����̍쐬
        Camera* CreateCamera(const std::string &name);

        // �J�����̎擾
        Camera* GetCamera(const std::string &name) const;

        // ���C���J�����̐ݒ�
        void SetMainCamera(const std::string &name);

        // ���C���J�����̎擾
        Camera* GetMainCamera() const { return pMainCamera_.get(); }

		void Uninit()
		{
            if (isUninited)return;
			cameras_.clear();
			pMainCamera_.reset();
            isUninited = true;
		}

    private:
        CameraManager() = default;
        ~CameraManager()
        {
			Uninit();           
        }

        bool isUninited = false;

        CameraManager(const CameraManager &) = delete;
        CameraManager &operator=(const CameraManager &) = delete;


        std::unordered_map<std::string, std::shared_ptr<Camera>> cameras_; // �J�����̃��X�g
        std::shared_ptr<Camera> pMainCamera_; // ���C���J����
	};
}