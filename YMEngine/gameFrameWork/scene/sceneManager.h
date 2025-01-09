#pragma once



namespace ym {

	class BaseScene;

    class SceneManager {
    public:
        // �V���O���g���C���X�^���X�̎擾
        static SceneManager *Instance() {
            if (m_instance == nullptr)
                m_instance = new SceneManager();

            return m_instance;
        }

        // �R�s�[�֎~
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

        // �V�[���̐؂�ւ�
        void ChangeScene(std::unique_ptr<BaseScene> scene);

        // �V�[���̒ǉ��i�X�^�b�N�Ƀv�b�V���j
        void PushScene(std::unique_ptr<BaseScene> scene);

        // ���݂̃V�[�����폜�i�X�^�b�N����|�b�v�j
        void PopScene();

		// ���݂̃V�[�����X�V
		void FixedUpdate();

		// ���݂̃V�[�����t���[�����[�g�ɓ������čX�V
        void Update();

        // ���݂̃V�[����`��
        void Draw();

		// �V�[���}�l�[�W���[�̏I������
		void Terminate();

    private:
        SceneManager();
        ~SceneManager();

        std::stack<std::unique_ptr<BaseScene>> m_pScene; // �V�[���̃X�^�b�N
        static SceneManager *m_instance;

    };

#define GetSceneInstance SceneManager::Instance()

} // namespace ym

