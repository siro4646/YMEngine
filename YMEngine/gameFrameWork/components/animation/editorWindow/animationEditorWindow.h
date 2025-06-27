#pragma once
#include "../animator/animator.h"

namespace ym
{
    struct KeySelection
    {
        std::string propertyKey;   // 例: "Cube1:Transform.Position.x"
        float       time = -1.0f;
    };

    class AnimationEditorWindow
    {
    public:
        void Draw(Animator *animator);

    private:
        /* ===== タイムライン関係 ===== */
        float timelineZoom = 1.0f;   // 拡大倍率
        float timelineOffset = 0.0f;   // 左端スクロール量
        float timelineHeight = 120.0f;

        /* ===== 編集対象クリップ／オブジェクト ===== */
        std::string selectedClip;
        std::string selectedObjectPath = "Cube1";

        /* ===== 再生カーソル ===== */
        float  cursorTime = 0.0f;
        bool   isPlaying = false;
        double playbackStartTime = 0.0;  // ImGui::GetTime() 基準
        float  playbackStartCursor = 0.0f; // 再生開始時カーソル

        /* ===== 追加されたプロパティ ===== */
        std::vector<std::string>               activeProperties;     // カーブを持つプロパティキー
        std::unordered_map<std::string, bool>  propertySelections;   // チェック状態

        /* ===== キーフレーム選択 ===== */
        std::unordered_map<std::string, KeySelection> selectedKeys;  // key = propertyKey
        float selectedKeyTime = -1.0f;

        /* ===== 描画ヘルパ ===== */
        void DrawTimeline(const std::shared_ptr<AnimationClip> &clip,
            const AnimationCurve &curve,
            const std::string &propertyKey);
        void DrawKeyframes(AnimationCurve &curve,
            const std::string &propertyKey);

        void DrawTimeline(const std::shared_ptr<AnimationClip> &clip);
        void  DrawKeyframes(
            AnimationCurve &curve,
            const std::string &propertyKey,
            int layerIndex // ← 追加
        );

        void DrawKeyframeEditorRow(
            const std::string &propertyKey,
            AnimationCurve &curve,
            float clipLength);

        std::string currentEditingClip;
        /* ===== 旧 Transform チャンネル UI を残したい場合に備えたダミー ===== */
        static constexpr const char *channelOptions[5] = { "posX","posY","posZ","rotY","sclZ" };
        bool channelSelections[5] = { true,true,true,false,false };  // ※ 互換用
    };
}
