#pragma once



namespace ym {

	class BaseScene;

    class SceneManager {
    public:
        // シングルトンインスタンスの取得
        static SceneManager *Instance() {
            if (m_instance == nullptr)
                m_instance = new SceneManager();

            return m_instance;
        }

        // コピー禁止
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

        // シーンの切り替え
        void ChangeScene(std::unique_ptr<BaseScene> scene);

        // シーンの追加（スタックにプッシュ）
        void PushScene(std::unique_ptr<BaseScene> scene);

        // 現在のシーンを削除（スタックからポップ）
        void PopScene();

		// 現在のシーンを更新
		void FixedUpdate();

		// 現在のシーンをフレームレートに同期して更新
        void Update();

        // 現在のシーンを描画
        void Draw();

		// シーンマネージャーの終了処理
		void Terminate();

    private:
        SceneManager();
        ~SceneManager();

        std::stack<std::unique_ptr<BaseScene>> m_pScene; // シーンのスタック
        static SceneManager *m_instance;

    };

#define GetSceneInstance SceneManager::Instance()

} // namespace ym

