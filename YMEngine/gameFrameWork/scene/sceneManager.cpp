#include "sceneManager.h"
#include "baseScene.h"
#include <iostream> // �f�o�b�O�p

#include "../Game/Scene/TestScene/testScene.h"

namespace ym {

	SceneManager *SceneManager::m_instance = nullptr;

    void SceneManager::ChangeScene(std::unique_ptr<BaseScene> scene) {
        if (!scene) {
            std::cerr << "Error: Nullptr provided to ChangeScene\n";
            return;
        }

        // ���݂̃V�[�����폜
        PopScene();

        // �V�����V�[�����v�b�V�����ď�����
        PushScene(std::move(scene));
    }

    void SceneManager::PushScene(std::unique_ptr<BaseScene> scene) {
        if (!scene) {
            std::cerr << "Error: Nullptr provided to PushScene\n";
            return;
        }

        // �V�����V�[�����X�^�b�N�ɒǉ����A������
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

        // ���݂̃V�[�����I�����A�X�^�b�N����폜
        m_pScene.top()->UnInit();
        m_pScene.pop();
    }

    void SceneManager::FixedUpdate()
    {
		if (m_pScene.empty()) {
			std::cerr << "Warning: FixedUpdate called with no scenes available\n";
			return;
		}

		// ���݂̃V�[�����X�V
		m_pScene.top()->FixedUpdate();
    }

    void SceneManager::Update() {
        if (m_pScene.empty()) {
            std::cerr << "Warning: Update called with no scenes available\n";
            return;
        }

        // ���݂̃V�[�����X�V
        m_pScene.top()->Update();
    }

    void SceneManager::Draw() {
        if (m_pScene.empty()) {
            std::cerr << "Warning: Draw called with no scenes available\n";
            return;
        }

        // ���݂̃V�[����`��
        m_pScene.top()->Draw();
    }
    SceneManager::SceneManager()
    {
		// �e�X�g�V�[����ǉ�
		PushScene(std::make_unique<TestScene>());		
    }
    void SceneManager::Terminate()
    {
        int i = 0;
        while (!m_pScene.empty())
        {
            i++;
            ym::ConsoleLog("�V�[���폜 %d\n", i);
            if (m_pScene.top() != nullptr)
            {
                m_pScene.top()->UnInit();
                m_pScene.pop();
            }
        }

        ym::ConsoleLog("�V�[���폜����\n");
        //���g���폜
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
