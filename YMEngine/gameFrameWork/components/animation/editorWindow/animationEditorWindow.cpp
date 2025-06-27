#include "animationEditorWindow.h"
#include "application/application.h"

namespace ym
{
    /*������������������������������������������������������������������������������������������������������������������������������*/
    void AnimationEditorWindow::Draw(Animator *animator)
        /*������������������������������������������������������������������������������������������������������������������������������*/
    {
        ImGui::Begin("Animation Editor");
        if (!animator || !animator->editMode)
        {
            ImGui::TextDisabled("Editor inactive");
            ImGui::End();
            return;
        }

        /* ---------- �N���b�v�I�� ---------- */
        if (animator->editingClipName.empty())
            animator->editingClipName = animator->GetCurrentClipName();



        std::vector<std::string> clipNames = animator->GetAvailableClipNames();
        clipNames.push_back("CreateNewClip");

        int selIdx = int(std::find(clipNames.begin(),
            clipNames.end(),
            animator->editingClipName) - clipNames.begin());

        if (ImGui::Combo("Select Clip", &selIdx,
            [](void *data, int idx, const char **out)
            {
                const auto &v = *static_cast<std::vector<std::string>*>(data);
                if (idx < 0 || idx >= v.size()) return false;
                *out = v[idx].c_str();
                return true;
            }, &clipNames, int(clipNames.size())))
        {
            animator->editingClipName = clipNames[selIdx];
        }

        const std::string &clipNameNow = animator->editingClipName;

        /* ���N���b�v���ς�������x UI ��Ԃ��N���A���遚 */
        if (clipNameNow != currentEditingClip)
        {
            activeProperties.clear();
            propertySelections.clear();
            selectedKeys.clear();
            selectedKeyTime = -1.0f;
            cursorTime = 0.0f;
            currentEditingClip = clipNameNow;

            // �ǉ��F�����N���b�v�Ɋ܂܂��J�[�u�����ɏ����\���v���p�e�B���Z�b�g
            auto clip = animator->GetClip(clipNameNow);
            if (clip)
            {
                for (auto &[propKey, _] : clip->GetAllCurves())
                {
                    activeProperties.push_back(propKey);
                    propertySelections[propKey] = true;
                }
            }
			if (clipNameNow != "CreateNewClip")
            {
				// �I�����ꂽ�N���b�v�̃t�@�C������ݒ�
                animator->SetFileName(clipNameNow);
            }
        }

        /* --- �V�K�N���b�v�쐬 --- */
        if (clipNames[selIdx] == "CreateNewClip")
        {
            static char newName[64] = "UnnamedClip";
            ImGui::InputText("New Clip Name", newName, IM_ARRAYSIZE(newName));
            if (ImGui::Button("Create Clip"))
            {
                if (!animator->HasClip(newName))
                {
                    std::string base = newName;
                    int n = 0;
                    std::string finalName;
                    do { 
                        finalName = base + std::to_string(n++);
                    } while (animator->HasClip(finalName));

                    animator->AddClip(finalName, std::make_shared<AnimationClip>());
                    animator->editingClipName = finalName;

                    activeProperties.clear();
                    propertySelections.clear();
                    selectedKeys.clear();
                    selectedKeyTime = -1.0f;
                    cursorTime = 0.0f;
                    currentEditingClip = finalName;
                    animator->SetFileName(finalName);
                }
            }
            ImGui::End();
            return;
        }

        /* ---------- �N���b�v�擾 ---------- */
        selectedClip = animator->editingClipName;
        auto clip = animator->GetClip(selectedClip);
        if (!clip) { ImGui::End(); return; }

        /* ---------- �Đ��R���g���[�� ---------- */
        ImGui::SliderFloat("Time", &cursorTime, 0.0f, clip->length, "%.2f");
        ImGui::DragFloat("Clip Length", &clip->length, 0.1f, 0.0f, 120.0f, "%.2f");
        clip->length = std::max(clip->length, 0.01f);

        static bool loopPlayback = false;               // �� ���[�v�Đ��̃`�F�b�N
        static float lastCursorTimeBeforePlay = 0.0f;   // �� �Đ��O�̃J�[�\�����Ԃ�ۑ�


        ImGui::Checkbox("Loop", &loopPlayback);  // �� �`�F�b�N�{�b�N�X�ǉ�

        if (ImGui::Button(isPlaying ? "Pause" : "Play"))
        {
            if (isPlaying)
            {
                isPlaying = false;
            }
            else
            {
                isPlaying = true;
                playbackStartTime = ImGui::GetTime();
                playbackStartCursor = cursorTime;
                lastCursorTimeBeforePlay = cursorTime; // �� �Đ��O�̎��Ԃ�ۑ�
            }
        }

        // �Đ����Ȃ�J�[�\�����Ԃ�i�߂�
        if (isPlaying)
        {
            double elapsed = ImGui::GetTime() - playbackStartTime;
            cursorTime = playbackStartCursor + static_cast<float>(elapsed);

            if (cursorTime > clip->length)
            {
                if (loopPlayback)
                {
                    // ���[�v�Đ��F�Ăѐ擪�ɖ߂�
                    playbackStartTime = ImGui::GetTime();
                    playbackStartCursor = 0.0f;
                    cursorTime = 0.0f;
                }
                else
                {
                    // �񃋁[�v�F�Đ���~�����ɖ߂�
                    cursorTime = lastCursorTimeBeforePlay; // �� �Đ��O�ɖ߂�
                    isPlaying = false;
                }
            }

            animator->Evaluate(selectedClip, cursorTime); // �]�����ăA�j���[�V�������f
        }
        if (!isPlaying)
        {
            animator->Evaluate(selectedClip, cursorTime);
        }

        /* ==============================================================
           1) �ΏۃI�u�W�F�N�g & PropertyProvider �擾
        =================================================================*/
        Object *rootObj = animator->object;
        Object *target = rootObj ? animator->FindObjectByPath(selectedObjectPath)
            : nullptr;

        if (!target)
        {
            ImGui::TextDisabled("No target selected");
            ImGui::End();
            return;
        }

        /* ---------- Add Property �|�b�v�A�b�v ---------- */
        if (ImGui::Button("Add Property"))
            ImGui::OpenPopup("AddPropPopup");

        if (ImGui::BeginPopup("AddPropPopup"))
        {
            for (auto &provider : target->CollectPropertyProviders())
            {
                ImGui::SeparatorText(provider->GetProviderName().c_str());
                for (auto &prop : provider->GetProperties())
                {
                    std::string fullKey = selectedObjectPath + ":" + prop.displayName;
                    if (std::find(activeProperties.begin(),
                        activeProperties.end(),
                        fullKey) == activeProperties.end())
                    {
                        if (ImGui::Selectable(prop.displayName.c_str()))
                        {
                            activeProperties.push_back(fullKey);
                            propertySelections[fullKey] = true;

                            /* Curve �������������Ă����i�d���`�F�b�N�ς݁j */
                            clip->AddCurveForProperty(fullKey);
                        }
                    }
                }
            }
            ImGui::EndPopup();
        }

        /* ---------- �v���p�e�B���X�g�i�`�F�b�N�{�b�N�X�j ---------- */
        ImGui::Text("Active Properties:");
        for (auto &key : activeProperties)
        {
            bool *flag = &propertySelections[key];
            ImGui::Checkbox(key.c_str(), flag);
        }

        /* ==============================================================
           2) �L�[�ǉ� / �폜
        =================================================================*/
        /* ==============================================================
   2) �L�[�ǉ� / �폜
=================================================================*/
        if (ImGui::Button("Add Key"))
        {
            // �\�\�\ �@ �����Ώۂ̓A�N�e�B�u & �`�F�b�N�ς݃v���p�e�B�̂� �\�\�\
            for (auto &key : activeProperties)
            {
                if (!propertySelections[key]) continue;               // ��\���v���p�e�B�͖���

                // �Ή�����J�[�u��K���擾�i���݂��Ȃ���ΐ����j
                AnimationCurve &cv = clip->AddCurveForProperty(key);

                // ���łɓ������L�[�����邩�H�i�덷 1e-4f�j
                if (cv.HasKey(cursorTime)) continue;           // �_�u��h�~

                // -----------------------------------------------------------------
                // �A �u�����l�v������
                // -----------------------------------------------------------------
                float newValue;
                if (!cv.keyframes.empty())
                {
                    // �A-A �J�[�u�����ɃL�[������ �� ��Ԓl�iEvaluate�j���̗p
                    newValue = cv.Evaluate(cursorTime);
                }
                else
                {
                    // �A-B �܂��L�[������ �� �I�u�W�F�N�g�̃��A���^�C���l���̗p
                    size_t d = key.find(':');
                    std::string propName = key.substr(d + 1);

                    newValue = 0.0f;
                    for (auto &prov : target->CollectPropertyProviders())
                    {
                        for (auto &prop : prov->GetProperties())
                        {
                            if (prop.displayName == propName)
                            {
                                newValue = *prop.valuePtr;
                                break;
                            }
                        }
                    }
                }

                // -----------------------------------------------------------------
                // �B �L�[�ǉ�
                // -----------------------------------------------------------------
                cv.AddKey(cursorTime, newValue);

                // �C �I����Ԃ��X�V�i�����ҏW�ł���悤�Ɂj
                selectedKeys[key] = { key, cursorTime };
                selectedKeyTime = cursorTime;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete Key"))
        {
            for (auto it = selectedKeys.begin(); it != selectedKeys.end(); )
            {
                const std::string &key = it->first;
                float t = it->second.time;

                if (!propertySelections[key]) { ++it; continue; }

                if (auto *curve = clip->GetCurveForProperty(key))
                {
                    // �덷���e���č폜
                    auto &keys = curve->keyframes;
                    for (auto kIt = keys.begin(); kIt != keys.end(); ++kIt)
                    {
                        if (std::abs(kIt->first - t) < 1e-4f)
                        {
                            keys.erase(kIt);
                            break;
                        }
                    }
                }

                // �I����Ԃ�����폜
                it = selectedKeys.erase(it);
            }

            selectedKeyTime = -1.0f; // �Â��ėp�I��ϐ���������
        }


        /* ==============================================================
           3) �^�C�����C�� & �L�[�t���[���`��
        =================================================================*/
        //for (auto &key : activeProperties)
        //{
        //    if (!propertySelections[key]) continue;
        //    if (auto *curve = clip->GetCurveForProperty(key))
        //    {
        //        ImGui::SeparatorText(key.c_str());
        //        DrawTimeline(clip, *curve, key);
        //        DrawKeyframes(*curve, key);
        //    }
        //}

  //      ///* ==============================================================
  //      //   3) �^�C�����C�� & �L�[�t���[���`��
  //      //=================================================================*/
        for (auto &key : activeProperties)
        {
            if (!propertySelections[key]) continue;
            if (auto *curve = clip->GetCurveForProperty(key))
            {
                DrawKeyframeEditorRow(key, *curve, clip->length);
            }
        }

        ImGui::End();
    }

    ///*������������������������������������������������������������������������������������������������������������������������������*/
    //void AnimationEditorWindow::DrawTimeline(
    //    const std::shared_ptr<AnimationClip> &clip,
    //    const AnimationCurve & /*ignored*/,
    //    const std::string &propertyKey)
    //    /*������������������������������������������������������������������������������������������������������������������������������*/
    //{
    //    /* �V���v���Ȗڐ��\���i���ʂȂ̂� propertyKey �͖��g�p�j */
    //    ImVec2 start = ImGui::GetCursorScreenPos();
    //    ImVec2 size = { ImGui::GetContentRegionAvail().x, timelineHeight };
    //    ImDrawList *dl = ImGui::GetWindowDrawList();
    //    dl->AddRectFilled(start, start + size, IM_COL32(30, 30, 30, 255));

    //    for (float t = 0; t < clip->length; t += 0.1f)
    //    {
    //        float x = start.x + (t * 100.0f * timelineZoom) - timelineOffset;
    //        dl->AddLine({ x,start.y }, { x,start.y + size.y }, IM_COL32(80, 80, 80, 255));
    //    }

    //    ImGui::InvisibleButton(("tl_" + propertyKey).c_str(), size,
    //        ImGuiButtonFlags_MouseButtonLeft);
    //    if (ImGui::IsItemClicked())
    //    {
    //        float clickX = ImGui::GetIO().MousePos.x - start.x + timelineOffset;
    //        cursorTime = std::clamp(clickX / (100.0f * timelineZoom), 0.0f, clip->length);
    //        selectedKeyTime = cursorTime;  // �N���b�N�����ʒu��I���L�[�Ƃ��Đݒ�
    //    }

    //    /* �Đ��J�[�\�� */
    //    float cx = start.x + (cursorTime * 100.0f * timelineZoom) - timelineOffset;
    //    dl->AddLine({ cx,start.y }, { cx,start.y + size.y }, IM_COL32(255, 0, 0, 255));
    //}

    ///*������������������������������������������������������������������������������������������������������������������������������*/
    //void AnimationEditorWindow::DrawKeyframes(
    //    AnimationCurve &curve,
    //    const std::string &propertyKey)
    //    /*������������������������������������������������������������������������������������������������������������������������������*/
    //{
    //    ImVec2 base = ImGui::GetCursorScreenPos();
    //    ImDrawList *dl = ImGui::GetWindowDrawList();

    //    int idx = 0;
    //    for (auto &[t, v] : curve.keyframes)
    //    {
    //        float x = base.x + (t * 100.0f * timelineZoom) - timelineOffset;
    //        float y = base.y + timelineHeight * 0.5f;

    //        ImVec2 center(x, y);
    //        float r = 5.0f;

    //        /* �N���b�N�͈� */
    //        ImGui::SetCursorScreenPos(center - ImVec2(7, 7));
    //        ImGui::InvisibleButton(("kf_" + propertyKey + std::to_string(idx)).c_str(),
    //            { 14,14 });

    //        if (ImGui::IsItemClicked())
    //            selectedKeys[propertyKey] = { propertyKey, t };

    //        bool isSel = selectedKeys.count(propertyKey) &&
    //            std::abs(selectedKeys[propertyKey].time - t) < 1e-3f;
    //        dl->AddCircleFilled(center, r,
    //            isSel ? IM_COL32(255, 0, 0, 255)
    //            : IM_COL32(255, 255,255, 255));
    //        ++idx;
    //    }

    //    ImGui::Dummy({ 0, timelineHeight * 0.25f });
    //    ImGui::Separator();
    //    // ---- �I�����ꂽ�L�[�̒l�ҏW ----
    //    if (selectedKeys.count(propertyKey))
    //    {
    //        auto &sel = selectedKeys[propertyKey];
    //        // �L�[��T���i���Ԉ�v�}�덷�j
    //        auto kfIt = std::find_if(curve.keyframes.begin(), curve.keyframes.end(),
    //            [&](const auto &kv) { return std::abs(kv.first - sel.time) < 1e-4f; });

    //        if (kfIt != curve.keyframes.end())
    //        {
    //            // �ҏW�p�̃��[�J���ϐ�
    //            float editableTime = kfIt->first;
    //            float editableValue = kfIt->second;
    //            selectedKeyTime = editableTime; // �I�����ꂽ�L�[�̎��Ԃ��X�V
    //            std::string uniqueId = propertyKey + std::to_string(idx);
    //            DebugLog("Editing key: %s", uniqueId.c_str());
    //            ImGui::PushID(uniqueId.c_str());

    //            // ���� ���ԕҏW ������������������������������������������
    //            if (ImGui::InputFloat("Selected Key Time", &editableTime, 0.01f, 0.1f, "%.3f"))
    //            {
    //                bool timeChanged = std::abs(editableTime - kfIt->first) > 1e-4f;
    //                bool timeExists = curve.HasKey(editableTime);

    //                if (timeChanged && !timeExists)
    //                {
    //                    // ��U���݂̒l��ޔ�
    //                    float prevValue = kfIt->second;

    //                    // �폜
    //                    curve.keyframes.erase(kfIt);

    //                    // �V�K�}��
    //                    curve.AddKey(editableTime, prevValue);
    //                    sel.time = editableTime;

    //                    // ���S�ɍĎ擾���Ă����i������ it ���g���Ď��ɐi�ށj
    //                    kfIt = std::find_if(curve.keyframes.begin(), curve.keyframes.end(),
    //                        [&](const auto &kv) { return std::abs(kv.first - editableTime) < 1e-4f; });
    //                }
    //            }

    //            // ���� �l�ҏW ����������������������������������������������
    //            if (kfIt != curve.keyframes.end())
    //            {
    //                if (ImGui::InputFloat("Selected Key Value", &editableValue, 0.01f, 0.1f, "%.3f"))
    //                {
    //                    kfIt->second = editableValue; // �� ���S�ɏ㏑��
    //                }
    //            }


    //            // PopID: �K���Ō�ɑΉ�����Pop
    //            ImGui::PopID();

    //        }
    //        else
    //        {
    //            ImGui::Text("No key selected");
    //        }
    //    }

    //}

    //void AnimationEditorWindow::DrawTimeline(const std::shared_ptr<AnimationClip> &clip)
    //{
    //    ImVec2 start = ImGui::GetCursorScreenPos();
    //    ImVec2 size = { ImGui::GetContentRegionAvail().x, timelineHeight };
    //    ImDrawList *dl = ImGui::GetWindowDrawList();
    //    dl->AddRectFilled(start, start + size, IM_COL32(30, 30, 30, 255));

    //    for (float t = 0; t < clip->length; t += 0.1f)
    //    {
    //        float x = start.x + (t * 100.0f * timelineZoom) - timelineOffset;
    //        dl->AddLine({ x,start.y }, { x,start.y + size.y }, IM_COL32(80, 80, 80, 255));
    //    }

    //    ImGui::InvisibleButton("Timeline", size, ImGuiButtonFlags_MouseButtonLeft);
    //    if (ImGui::IsItemClicked())
    //    {
    //        float clickX = ImGui::GetIO().MousePos.x - start.x + timelineOffset;
    //        cursorTime = std::clamp(clickX / (100.0f * timelineZoom), 0.0f, clip->length);
    //        selectedKeyTime = cursorTime;
    //    }

    //    // �Đ��J�[�\��
    //    float cx = start.x + (cursorTime * 100.0f * timelineZoom) - timelineOffset;
    //    dl->AddLine({ cx,start.y }, { cx,start.y + size.y }, IM_COL32(255, 0, 0, 255));
    //}
    //void AnimationEditorWindow::DrawKeyframes(AnimationCurve &curve, const std::string &propertyKey, int layer)
    //{
    //    ImVec2 base = ImGui::GetCursorScreenPos();
    //    ImDrawList *dl = ImGui::GetWindowDrawList();

    //    int idx = 0;
    //    for (auto &[t, v] : curve.keyframes)
    //    {
    //        float x = base.x + (t * 100.0f * timelineZoom) - timelineOffset;
    //        float y = base.y + timelineHeight * layer + timelineHeight * 0.5f;

    //        ImVec2 center(x, y);
    //        float r = 5.0f;

    //        ImGui::SetCursorScreenPos(center - ImVec2(7, 7));
    //        ImGui::InvisibleButton(("kf_" + propertyKey + std::to_string(idx)).c_str(), { 14,14 });

    //        if (ImGui::IsItemClicked())
    //            selectedKeys[propertyKey] = { propertyKey, t };

    //        bool isSel = selectedKeys.count(propertyKey) &&
    //            std::abs(selectedKeys[propertyKey].time - t) < 1e-3f;

    //        dl->AddCircleFilled(center, r, isSel ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 0, 150));
    //        ++idx;
    //    }

    //    // ���C���[���Ƃ̕������i���������邱�Ƃ�Unity���Ɂj
    //    ImGui::Dummy({ 0, 2.0f });
    //}

    void AnimationEditorWindow::DrawKeyframeEditorRow(
        const std::string &propertyKey,
        AnimationCurve &curve,
        float clipLength)
    {
        ImGui::PushID(propertyKey.c_str());

        // === 1�s�ځF�v���p�e�B�� + �^�C�����C���o�[ ===
        ImGui::TextUnformatted(propertyKey.c_str());
        ImGui::SameLine(150.0f); // ���x���ʒu����

        ImVec2 start = ImGui::GetCursorScreenPos();
        ImVec2 size = { ImGui::GetContentRegionAvail().x, 20.0f };
        ImDrawList *dl = ImGui::GetWindowDrawList();

        // �^�C�����C���w�i
        dl->AddRectFilled(start, start + size, IM_COL32(40, 40, 40, 255));

        // �L�[�t���[���̕`��
        int idx = 0;
        for (auto &[t, v] : curve.keyframes)
        {
            float x = start.x + (t * 100.0f * timelineZoom) - timelineOffset;
            float y = start.y + size.y / 2;

            ImVec2 center(x, y);
            bool isSelected = selectedKeys.count(propertyKey) &&
                std::abs(selectedKeys[propertyKey].time - t) < 1e-4f;

            // �F�ύX
            ImU32 color = isSelected ? IM_COL32(50, 150, 255, 255) : IM_COL32(255, 255, 255, 255);
            dl->AddCircleFilled(center, 4.0f, color);

            ImGui::SetCursorScreenPos(center - ImVec2(6, 6));
            ImGui::InvisibleButton(("key_" + std::to_string(idx)).c_str(), { 12,12 });

            if (ImGui::IsItemClicked())
            {
                bool isCtrlPressed = ImGui::GetIO().KeyCtrl;
                float keyTime = t;

                if (isCtrlPressed)
                {
                    // Ctrl�����Ȃ���N���b�N �� �I���g�O��
                    auto it = selectedKeys.find(propertyKey);
                    if (it != selectedKeys.end() && std::abs(it->second.time - keyTime) < 1e-4f)
                    {
                        selectedKeys.erase(it); // �I������
                    }
                    else
                    {
                        selectedKeys[propertyKey] = { propertyKey, keyTime }; // �I��ǉ�
                    }
                }
                else
                {
                    // �ʏ�N���b�N �� ���ꂾ���I��
                    selectedKeys.clear();
                    selectedKeys[propertyKey] = { propertyKey, keyTime };
                }

                selectedKeyTime = keyTime; // �݊��p�̕ϐ����X�V
            }


            ++idx;
        }

        // �Đ��J�[�\��
        float cx = start.x + (cursorTime * 100.0f * timelineZoom) - timelineOffset;
        dl->AddLine({ cx, start.y }, { cx, start.y + size.y }, IM_COL32(255, 0, 0, 255));

        ImGui::Dummy({ 0, 24.0f }); // �^�C�����C���Ɠ��͗��̃X�y�[�X

        // ���͒����ǂ����𔻒肷�邽�߂̃t���O
        bool isActiveInput = ImGui::IsItemActive();
        static std::unordered_map<std::string, float> inputBuffer;

        float &buf = inputBuffer[propertyKey];

        // �\���l�� Evaluate �ōX�V�i���������͒��łȂ���΁j
        if (!ImGui::IsItemActive())
        {
            buf = curve.Evaluate(cursorTime);
        }

        if (ImGui::InputFloat("Value", &buf, 0.01f, 0.1f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            bool inserted = false;

            // �L�[�̑��݊m�F�i�덷���e�j
            auto it = std::find_if(curve.keyframes.begin(), curve.keyframes.end(),
                [&](const auto &kv) { return std::abs(kv.first - cursorTime) < 1e-4f; });

            if (it != curve.keyframes.end())
            {
                // �����L�[������ �� �㏑��
                it->second = buf;
            }
            else
            {
                // �����L�[���Ȃ� �� �ǉ�
                curve.AddKey(cursorTime, buf);
                inserted = true;
            }

            // �I����Ԃ��X�V
            selectedKeys[propertyKey] = { propertyKey, cursorTime };
            selectedKeyTime = cursorTime;

            DebugLog("%s: Key %s at time=%.3f, value=%.3f",
                propertyKey.c_str(),
                inserted ? "added" : "updated",
                cursorTime, buf);
        }



        ImGui::Dummy({ 0, 10.0f }); // �s�ԃX�y�[�X
        ImGui::PopID();
    }



}// namespace ym
