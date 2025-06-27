#pragma once
#include "gameFrameWork/components/component.h"
#include "../clip/animationClip.h"

namespace ym
{
    class AnimationEditorWindow;

    // �A�j���[�V�����Đ��𐧌䂷��R���|�[�l���g
    class Animator : public Component
    {
    public:
        using Component::Component;

        void Init() override;
        void Update() override;
        void FixedUpdate() override {}
        void Draw() override {}
        void Uninit() override;
        void DrawImguiBody() override;

        const char *GetName() const override { return "Animator"; }

		const string &GetCurrentClipName() const { return currentClip_; }

        // �A�j���[�V�����N���b�v�̊Ǘ�
        void AddClip(const std::string &name, std::shared_ptr<AnimationClip> clip);
        bool HasClip(const std::string &name) const;
        std::shared_ptr<AnimationClip> GetClip(const std::string &name) const;
        std::vector<std::string> GetAvailableClipNames() const;

        // �Đ�����
        void Play(const std::string &name, bool loop = false);
        void Stop();
        void Evaluate(std::string_view clipName, float time);  // �P���]��

        // �p�X�w��ɂ��ΏۃI�u�W�F�N�g�̌���
        Object *FindObjectByPath(const std::string &path);

        // �ҏW�t���O�E�ݒ�
        bool editMode = false;
        std::string editingClipName;
        bool useRootMotion = false;

        void SetFileName(const std::string &filename)
        {
            std::string fullPath = "asset/anim/" + filename + ".json";
            strncpy(filepathBuffer, fullPath.c_str(), sizeof(filepathBuffer) - 1);
            filepathBuffer[sizeof(filepathBuffer) - 1] = '\0';
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<AnimationClip>> clips_;
        std::string currentClip_;
        float currentTime_ = 0.0f;
        bool isPlaying_ = false;
        bool isLooping_ = false;

        // RootMotion �̃I�t�Z�b�g�p�����l�ipropertyKey -> �J�n�l�j
        std::unordered_map<std::string, float> rootStartValueMap_;

        void ApplyClip(std::shared_ptr<AnimationClip> clip, float time);

        AnimationEditorWindow *editorWindow = nullptr;
        char filepathBuffer[256] = "asset/anim/sample.json";
    };

    REGISTER_COMPONENT(Animator);
}
