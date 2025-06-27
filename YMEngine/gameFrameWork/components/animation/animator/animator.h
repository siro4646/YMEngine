#pragma once
#include "gameFrameWork/components/component.h"
#include "../clip/animationClip.h"

namespace ym
{
    class AnimationEditorWindow;

    // アニメーション再生を制御するコンポーネント
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

        // アニメーションクリップの管理
        void AddClip(const std::string &name, std::shared_ptr<AnimationClip> clip);
        bool HasClip(const std::string &name) const;
        std::shared_ptr<AnimationClip> GetClip(const std::string &name) const;
        std::vector<std::string> GetAvailableClipNames() const;

        // 再生制御
        void Play(const std::string &name, bool loop = false);
        void Stop();
        void Evaluate(std::string_view clipName, float time);  // 単発評価

        // パス指定による対象オブジェクトの検索
        Object *FindObjectByPath(const std::string &path);

        // 編集フラグ・設定
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

        // RootMotion のオフセット用初期値（propertyKey -> 開始値）
        std::unordered_map<std::string, float> rootStartValueMap_;

        void ApplyClip(std::shared_ptr<AnimationClip> clip, float time);

        AnimationEditorWindow *editorWindow = nullptr;
        char filepathBuffer[256] = "asset/anim/sample.json";
    };

    REGISTER_COMPONENT(Animator);
}
