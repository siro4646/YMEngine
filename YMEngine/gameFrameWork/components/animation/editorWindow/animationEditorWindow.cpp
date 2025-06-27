#include "animationEditorWindow.h"
#include "application/application.h"

namespace ym
{
    /*───────────────────────────────────────────────────────────────*/
    void AnimationEditorWindow::Draw(Animator *animator)
        /*───────────────────────────────────────────────────────────────*/
    {
        ImGui::Begin("Animation Editor");
        if (!animator || !animator->editMode)
        {
            ImGui::TextDisabled("Editor inactive");
            ImGui::End();
            return;
        }

        /* ---------- クリップ選択 ---------- */
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

        /* ★クリップが変わったら一度 UI 状態をクリアする★ */
        if (clipNameNow != currentEditingClip)
        {
            activeProperties.clear();
            propertySelections.clear();
            selectedKeys.clear();
            selectedKeyTime = -1.0f;
            cursorTime = 0.0f;
            currentEditingClip = clipNameNow;

            // 追加：既存クリップに含まれるカーブを元に初期表示プロパティをセット
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
				// 選択されたクリップのファイル名を設定
                animator->SetFileName(clipNameNow);
            }
        }

        /* --- 新規クリップ作成 --- */
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

        /* ---------- クリップ取得 ---------- */
        selectedClip = animator->editingClipName;
        auto clip = animator->GetClip(selectedClip);
        if (!clip) { ImGui::End(); return; }

        /* ---------- 再生コントロール ---------- */
        ImGui::SliderFloat("Time", &cursorTime, 0.0f, clip->length, "%.2f");
        ImGui::DragFloat("Clip Length", &clip->length, 0.1f, 0.0f, 120.0f, "%.2f");
        clip->length = std::max(clip->length, 0.01f);

        static bool loopPlayback = false;               // ← ループ再生のチェック
        static float lastCursorTimeBeforePlay = 0.0f;   // ← 再生前のカーソル時間を保存


        ImGui::Checkbox("Loop", &loopPlayback);  // ← チェックボックス追加

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
                lastCursorTimeBeforePlay = cursorTime; // ← 再生前の時間を保存
            }
        }

        // 再生中ならカーソル時間を進める
        if (isPlaying)
        {
            double elapsed = ImGui::GetTime() - playbackStartTime;
            cursorTime = playbackStartCursor + static_cast<float>(elapsed);

            if (cursorTime > clip->length)
            {
                if (loopPlayback)
                {
                    // ループ再生：再び先頭に戻す
                    playbackStartTime = ImGui::GetTime();
                    playbackStartCursor = 0.0f;
                    cursorTime = 0.0f;
                }
                else
                {
                    // 非ループ：再生停止＆元に戻す
                    cursorTime = lastCursorTimeBeforePlay; // ← 再生前に戻す
                    isPlaying = false;
                }
            }

            animator->Evaluate(selectedClip, cursorTime); // 評価してアニメーション反映
        }
        if (!isPlaying)
        {
            animator->Evaluate(selectedClip, cursorTime);
        }

        /* ==============================================================
           1) 対象オブジェクト & PropertyProvider 取得
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

        /* ---------- Add Property ポップアップ ---------- */
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

                            /* Curve を強制生成しておく（重複チェック済み） */
                            clip->AddCurveForProperty(fullKey);
                        }
                    }
                }
            }
            ImGui::EndPopup();
        }

        /* ---------- プロパティリスト（チェックボックス） ---------- */
        ImGui::Text("Active Properties:");
        for (auto &key : activeProperties)
        {
            bool *flag = &propertySelections[key];
            ImGui::Checkbox(key.c_str(), flag);
        }

        /* ==============================================================
           2) キー追加 / 削除
        =================================================================*/
        /* ==============================================================
   2) キー追加 / 削除
=================================================================*/
        if (ImGui::Button("Add Key"))
        {
            // ――― ① 走査対象はアクティブ & チェック済みプロパティのみ ―――
            for (auto &key : activeProperties)
            {
                if (!propertySelections[key]) continue;               // 非表示プロパティは無視

                // 対応するカーブを必ず取得（存在しなければ生成）
                AnimationCurve &cv = clip->AddCurveForProperty(key);

                // すでに同時刻キーがあるか？（誤差 1e-4f）
                if (cv.HasKey(cursorTime)) continue;           // ダブり防止

                // -----------------------------------------------------------------
                // ② 「初期値」を決定
                // -----------------------------------------------------------------
                float newValue;
                if (!cv.keyframes.empty())
                {
                    // ②-A カーブが既にキーを持つ → 補間値（Evaluate）を採用
                    newValue = cv.Evaluate(cursorTime);
                }
                else
                {
                    // ②-B まだキーが無い → オブジェクトのリアルタイム値を採用
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
                // ③ キー追加
                // -----------------------------------------------------------------
                cv.AddKey(cursorTime, newValue);

                // ④ 選択状態を更新（すぐ編集できるように）
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
                    // 誤差許容して削除
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

                // 選択状態からも削除
                it = selectedKeys.erase(it);
            }

            selectedKeyTime = -1.0f; // 古い汎用選択変数も無効化
        }


        /* ==============================================================
           3) タイムライン & キーフレーム描画
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
  //      //   3) タイムライン & キーフレーム描画
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

    ///*───────────────────────────────────────────────────────────────*/
    //void AnimationEditorWindow::DrawTimeline(
    //    const std::shared_ptr<AnimationClip> &clip,
    //    const AnimationCurve & /*ignored*/,
    //    const std::string &propertyKey)
    //    /*───────────────────────────────────────────────────────────────*/
    //{
    //    /* シンプルな目盛表示（共通なので propertyKey は未使用） */
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
    //        selectedKeyTime = cursorTime;  // クリックした位置を選択キーとして設定
    //    }

    //    /* 再生カーソル */
    //    float cx = start.x + (cursorTime * 100.0f * timelineZoom) - timelineOffset;
    //    dl->AddLine({ cx,start.y }, { cx,start.y + size.y }, IM_COL32(255, 0, 0, 255));
    //}

    ///*───────────────────────────────────────────────────────────────*/
    //void AnimationEditorWindow::DrawKeyframes(
    //    AnimationCurve &curve,
    //    const std::string &propertyKey)
    //    /*───────────────────────────────────────────────────────────────*/
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

    //        /* クリック範囲 */
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
    //    // ---- 選択されたキーの値編集 ----
    //    if (selectedKeys.count(propertyKey))
    //    {
    //        auto &sel = selectedKeys[propertyKey];
    //        // キーを探す（時間一致±誤差）
    //        auto kfIt = std::find_if(curve.keyframes.begin(), curve.keyframes.end(),
    //            [&](const auto &kv) { return std::abs(kv.first - sel.time) < 1e-4f; });

    //        if (kfIt != curve.keyframes.end())
    //        {
    //            // 編集用のローカル変数
    //            float editableTime = kfIt->first;
    //            float editableValue = kfIt->second;
    //            selectedKeyTime = editableTime; // 選択されたキーの時間を更新
    //            std::string uniqueId = propertyKey + std::to_string(idx);
    //            DebugLog("Editing key: %s", uniqueId.c_str());
    //            ImGui::PushID(uniqueId.c_str());

    //            // ── 時間編集 ─────────────────────
    //            if (ImGui::InputFloat("Selected Key Time", &editableTime, 0.01f, 0.1f, "%.3f"))
    //            {
    //                bool timeChanged = std::abs(editableTime - kfIt->first) > 1e-4f;
    //                bool timeExists = curve.HasKey(editableTime);

    //                if (timeChanged && !timeExists)
    //                {
    //                    // 一旦現在の値を退避
    //                    float prevValue = kfIt->second;

    //                    // 削除
    //                    curve.keyframes.erase(kfIt);

    //                    // 新規挿入
    //                    curve.AddKey(editableTime, prevValue);
    //                    sel.time = editableTime;

    //                    // 安全に再取得しておく（↓この it を使って次に進む）
    //                    kfIt = std::find_if(curve.keyframes.begin(), curve.keyframes.end(),
    //                        [&](const auto &kv) { return std::abs(kv.first - editableTime) < 1e-4f; });
    //                }
    //            }

    //            // ── 値編集 ───────────────────────
    //            if (kfIt != curve.keyframes.end())
    //            {
    //                if (ImGui::InputFloat("Selected Key Value", &editableValue, 0.01f, 0.1f, "%.3f"))
    //                {
    //                    kfIt->second = editableValue; // ← 安全に上書き
    //                }
    //            }


    //            // PopID: 必ず最後に対応するPop
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

    //    // 再生カーソル
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

    //    // レイヤーごとの分離幅（小さくすることでUnity風に）
    //    ImGui::Dummy({ 0, 2.0f });
    //}

    void AnimationEditorWindow::DrawKeyframeEditorRow(
        const std::string &propertyKey,
        AnimationCurve &curve,
        float clipLength)
    {
        ImGui::PushID(propertyKey.c_str());

        // === 1行目：プロパティ名 + タイムラインバー ===
        ImGui::TextUnformatted(propertyKey.c_str());
        ImGui::SameLine(150.0f); // ラベル位置調整

        ImVec2 start = ImGui::GetCursorScreenPos();
        ImVec2 size = { ImGui::GetContentRegionAvail().x, 20.0f };
        ImDrawList *dl = ImGui::GetWindowDrawList();

        // タイムライン背景
        dl->AddRectFilled(start, start + size, IM_COL32(40, 40, 40, 255));

        // キーフレームの描画
        int idx = 0;
        for (auto &[t, v] : curve.keyframes)
        {
            float x = start.x + (t * 100.0f * timelineZoom) - timelineOffset;
            float y = start.y + size.y / 2;

            ImVec2 center(x, y);
            bool isSelected = selectedKeys.count(propertyKey) &&
                std::abs(selectedKeys[propertyKey].time - t) < 1e-4f;

            // 色変更
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
                    // Ctrl押しながらクリック → 選択トグル
                    auto it = selectedKeys.find(propertyKey);
                    if (it != selectedKeys.end() && std::abs(it->second.time - keyTime) < 1e-4f)
                    {
                        selectedKeys.erase(it); // 選択解除
                    }
                    else
                    {
                        selectedKeys[propertyKey] = { propertyKey, keyTime }; // 選択追加
                    }
                }
                else
                {
                    // 通常クリック → それだけ選択
                    selectedKeys.clear();
                    selectedKeys[propertyKey] = { propertyKey, keyTime };
                }

                selectedKeyTime = keyTime; // 互換用の変数も更新
            }


            ++idx;
        }

        // 再生カーソル
        float cx = start.x + (cursorTime * 100.0f * timelineZoom) - timelineOffset;
        dl->AddLine({ cx, start.y }, { cx, start.y + size.y }, IM_COL32(255, 0, 0, 255));

        ImGui::Dummy({ 0, 24.0f }); // タイムラインと入力欄のスペース

        // 入力中かどうかを判定するためのフラグ
        bool isActiveInput = ImGui::IsItemActive();
        static std::unordered_map<std::string, float> inputBuffer;

        float &buf = inputBuffer[propertyKey];

        // 表示値を Evaluate で更新（ただし入力中でなければ）
        if (!ImGui::IsItemActive())
        {
            buf = curve.Evaluate(cursorTime);
        }

        if (ImGui::InputFloat("Value", &buf, 0.01f, 0.1f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            bool inserted = false;

            // キーの存在確認（誤差許容）
            auto it = std::find_if(curve.keyframes.begin(), curve.keyframes.end(),
                [&](const auto &kv) { return std::abs(kv.first - cursorTime) < 1e-4f; });

            if (it != curve.keyframes.end())
            {
                // 既存キーがある → 上書き
                it->second = buf;
            }
            else
            {
                // 既存キーがない → 追加
                curve.AddKey(cursorTime, buf);
                inserted = true;
            }

            // 選択状態を更新
            selectedKeys[propertyKey] = { propertyKey, cursorTime };
            selectedKeyTime = cursorTime;

            DebugLog("%s: Key %s at time=%.3f, value=%.3f",
                propertyKey.c_str(),
                inserted ? "added" : "updated",
                cursorTime, buf);
        }



        ImGui::Dummy({ 0, 10.0f }); // 行間スペース
        ImGui::PopID();
    }



}// namespace ym
