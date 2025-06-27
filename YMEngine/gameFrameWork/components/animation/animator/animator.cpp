#include "animator.h"
#include "application/application.h"
#include "../editorWindow/animationEditorWindow.h"

namespace ym
{
    void Animator::AddClip(const std::string &name, std::shared_ptr<AnimationClip> clip)
    {
        clips_[name] = clip;
    }

    bool Animator::HasClip(const std::string &name) const
    {
        return clips_.find(name) != clips_.end();
    }

    std::shared_ptr<AnimationClip> Animator::GetClip(const std::string &name) const
    {
        auto it = clips_.find(name);
        return (it != clips_.end()) ? it->second : nullptr;
    }

    std::vector<std::string> Animator::GetAvailableClipNames() const
    {
        std::vector<std::string> names;
        for (const auto &[name, _] : clips_) names.push_back(name);
        return names;
    }

    void Animator::Play(const std::string &name, bool loop)
    {
        if (!HasClip(name)) return;

        currentClip_ = name;
        currentTime_ = 0.0f;
        isLooping_ = loop;
        isPlaying_ = true;

        // RootMotion オフセット初期値の記録
        if (useRootMotion && object)
        {
            rootStartValueMap_.clear();
            auto clip = clips_[currentClip_];

            for (const auto &[propertyKey, curve] : clip->GetAllCurves())
            {
                std::string path = propertyKey.substr(0, propertyKey.find(':'));
                std::string propName = propertyKey.substr(propertyKey.find(':') + 1);

                Object *target = FindObjectByPath(path);
                if (!target) continue;

                auto providers = target->CollectPropertyProviders();
                for (auto &provider : providers)
                {
                    for (auto &prop : provider->GetProperties())
                    {
                        if (prop.displayName == propName)
                        {
                            rootStartValueMap_[propertyKey] = *prop.valuePtr;
                        }
                    }
                }
            }
        }
    }

    void Animator::Stop()
    {
        isPlaying_ = false;
        currentTime_ = 0.0f;
    }

    void Animator::Evaluate(std::string_view clipName, float time)
    {
        auto clip = GetClip(std::string(clipName));
        if (clip) ApplyClip(clip, time);
    }

    void Animator::ApplyClip(std::shared_ptr<AnimationClip> clip, float time)
    {
        for (const auto &[propertyKey, curve] : clip->GetAllCurves())
        {
            size_t delim = propertyKey.find(':');
            if (delim == std::string::npos) continue;

            std::string path = propertyKey.substr(0, delim);
            std::string propName = propertyKey.substr(delim + 1);

            Object *target = FindObjectByPath(path);
            if (!target) continue;

            float animValue = curve.Evaluate(time);

            auto providers = target->CollectPropertyProviders();
            for (auto &provider : providers)
            {
                for (auto &prop : provider->GetProperties())
                {
                    if (prop.displayName == propName)
                    {
                        if (useRootMotion && target == object && rootStartValueMap_.count(propertyKey))
                        {
                            *prop.valuePtr = rootStartValueMap_[propertyKey] + animValue;
                        }
                        else
                        {
                            *prop.valuePtr = animValue;
                        }
                    }
                }
            }
        }
    }

    void Animator::Init()
    {
        editorWindow = new AnimationEditorWindow();
    }

    void Animator::Uninit()
    {
        delete editorWindow;
    }

    void Animator::Update()
    {
        if (!isPlaying_ || !HasClip(currentClip_)) return;

        auto clip = clips_[currentClip_];
        currentTime_ += Application::Instance()->GetDeltaTime();

        // 再生終了チェック
        if (currentTime_ > clip->length)
        {
            if (isLooping_)
				Play(currentClip_, isLooping_);
            else
            {
                isPlaying_ = false;
                return;
            }
        }

        ApplyClip(clip, currentTime_);
    }

    void Animator::DrawImguiBody()
    {
        ImGui::Text("Current Clip: %s", currentClip_.c_str());
        ImGui::Checkbox("Loop", &isLooping_);
        ImGui::Checkbox("Use Root Motion", &useRootMotion);
        if (!editMode) {
            if (ImGui::Button("Play")) Play(currentClip_);
            ImGui::SameLine();
            if (ImGui::Button("Stop")) Stop();
        }
        ImGui::Text("Available Clips:");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        for (const auto &[name, _] : clips_)
        {
            if (ImGui::Selectable(name.c_str(), currentClip_ == name))
                currentClip_ = name;
        }
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::Text("Save/Load AnimationClip");
        ImGui::InputText("Path", filepathBuffer, IM_ARRAYSIZE(filepathBuffer));

        if (ImGui::Button("Save"))
        {
            if (auto it = clips_.find(currentClip_); it != clips_.end())
            {
                it->second->SaveToJsonFile(filepathBuffer);
                ym::ConsoleLog("Saved clip: %s\n", filepathBuffer);
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            auto clip = AnimationClip::LoadFromJsonFile(filepathBuffer);
            if (clip)
            {
                std::string name = std::filesystem::path(filepathBuffer).stem().string();
                clips_[name] = clip;
                currentClip_ = name;
                //Play(currentClip_, isLooping_);
                ym::ConsoleLog("Loaded clip: %s\n", filepathBuffer);
            }
        }

        ImGui::SameLine();
        ImGui::Checkbox("Edit", &editMode);
        if (editMode && editorWindow)
        {
            editorWindow->Draw(this);
        }
    }

    Object *Animator::FindObjectByPath(const std::string &path)
    {
        if (!object) return nullptr;

        std::stringstream ss(path);
        std::string segment;
        Object *current = object;

        if (!std::getline(ss, segment, '/')) return nullptr;
        if (segment != object->name) return nullptr;

        while (std::getline(ss, segment, '/'))
        {
            bool found = false;
            for (auto &child : current->_childs)
            {
                if (child->name == segment)
                {
                    current = child.get();
                    found = true;
                    break;
                }
            }
            if (!found) return nullptr;
        }

        return current;
    }
}
