#include "sceneManager.h"
#include "baseScene.h"
#include <iostream> // デバッグ用

#include "../Game/Scene/TestScene/testScene.h"

namespace ym {

	SceneManager *SceneManager::m_instance = nullptr;

    void SceneManager::ChangeScene(std::unique_ptr<BaseScene> scene) {
        if (!scene) {
            std::cerr << "Error: Nullptr provided to ChangeScene\n";
            return;
        }

        // 現在のシーンを削除
        PopScene();

        // 新しいシーンをプッシュして初期化
        PushScene(std::move(scene));
    }

    void SceneManager::PushScene(std::unique_ptr<BaseScene> scene) {
        if (!scene) {
            std::cerr << "Error: Nullptr provided to PushScene\n";
            return;
        }

        // 新しいシーンをスタックに追加し、初期化
        m_pScene.push(std::move(scene));
        try {
            m_pScene.top()->Init();
        }
        catch (const std::exception &e) {
            std::cerr << "Scene initialization failed: " << e.what() << '\n';
            m_pScene.pop();
        }
    }

    void SceneManager::PopScene() {
        if (m_pScene.empty()) {
            std::cerr << "Warning: PopScene called on empty stack\n";
            return;
        }

        // 現在のシーンを終了し、スタックから削除
        m_pScene.top()->UnInit();
        m_pScene.pop();
    }

    void SceneManager::FixedUpdate()
    {
		if (m_pScene.empty()) {
			std::cerr << "Warning: FixedUpdate called with no scenes available\n";
			return;
		}

		// 現在のシーンを更新
		m_pScene.top()->FixedUpdate();
    }

    void SceneManager::Update() {
        if (m_pScene.empty()) {
            std::cerr << "Warning: Update called with no scenes available\n";
            return;
        }

        // 現在のシーンを更新
        m_pScene.top()->Update();
    }

    void SceneManager::Draw() {
        if (m_pScene.empty()) {
            std::cerr << "Warning: Draw called with no scenes available\n";
            return;
        }

        // 現在のシーンを描画
        m_pScene.top()->Draw();
    }
    SceneManager::SceneManager()
    {
		// テストシーンを追加
		PushScene(std::make_unique<TestScene>());		
    }
    void SceneManager::Terminate()
    {
        int i = 0;
        while (!m_pScene.empty())
        {
            i++;
            ym::ConsoleLog("シーン削除 %d\n", i);
            if (m_pScene.top() != nullptr)
            {
                m_pScene.top()->UnInit();
                m_pScene.pop();
            }
        }

        ym::ConsoleLog("シーン削除完了\n");
        //自身を削除
        if (m_instance != nullptr)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }
    SceneManager::~SceneManager()
    {
			
    }
}
