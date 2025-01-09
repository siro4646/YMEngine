#pragma once

namespace ym 
{
	class Camera;

	class CameraManager
	{
    public:
        // シングルトンインスタンスの取得
        static CameraManager &Instance() 
        {
            static CameraManager instance;
            return instance;
        }

        // カメラの作成
        Camera* CreateCamera(const std::string &name);

        // カメラの取得
        Camera* GetCamera(const std::string &name) const;

        // メインカメラの設定
        void SetMainCamera(const std::string &name);

        // メインカメラの取得
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


        std::unordered_map<std::string, std::shared_ptr<Camera>> cameras_; // カメラのリスト
        std::shared_ptr<Camera> pMainCamera_; // メインカメラ
	};
}