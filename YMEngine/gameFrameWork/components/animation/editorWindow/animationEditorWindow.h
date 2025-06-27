#pragma once
#include "../animator/animator.h"

namespace ym
{
    struct KeySelection
    {
        std::string propertyKey;   // ��: "Cube1:Transform.Position.x"
        float       time = -1.0f;
    };

    class AnimationEditorWindow
    {
    public:
        void Draw(Animator *animator);

    private:
        /* ===== �^�C�����C���֌W ===== */
        float timelineZoom = 1.0f;   // �g��{��
        float timelineOffset = 0.0f;   // ���[�X�N���[����
        float timelineHeight = 120.0f;

        /* ===== �ҏW�ΏۃN���b�v�^�I�u�W�F�N�g ===== */
        std::string selectedClip;
        std::string selectedObjectPath = "Cube1";

        /* ===== �Đ��J�[�\�� ===== */
        float  cursorTime = 0.0f;
        bool   isPlaying = false;
        double playbackStartTime = 0.0;  // ImGui::GetTime() �
        float  playbackStartCursor = 0.0f; // �Đ��J�n���J�[�\��

        /* ===== �ǉ����ꂽ�v���p�e�B ===== */
        std::vector<std::string>               activeProperties;     // �J�[�u�����v���p�e�B�L�[
        std::unordered_map<std::string, bool>  propertySelections;   // �`�F�b�N���

        /* ===== �L�[�t���[���I�� ===== */
        std::unordered_map<std::string, KeySelection> selectedKeys;  // key = propertyKey
        float selectedKeyTime = -1.0f;

        /* ===== �`��w���p ===== */
        void DrawTimeline(const std::shared_ptr<AnimationClip> &clip,
            const AnimationCurve &curve,
            const std::string &propertyKey);
        void DrawKeyframes(AnimationCurve &curve,
            const std::string &propertyKey);

        void DrawTimeline(const std::shared_ptr<AnimationClip> &clip);
        void  DrawKeyframes(
            AnimationCurve &curve,
            const std::string &propertyKey,
            int layerIndex // �� �ǉ�
        );

        void DrawKeyframeEditorRow(
            const std::string &propertyKey,
            AnimationCurve &curve,
            float clipLength);

        std::string currentEditingClip;
        /* ===== �� Transform �`�����l�� UI ���c�������ꍇ�ɔ������_�~�[ ===== */
        static constexpr const char *channelOptions[5] = { "posX","posY","posZ","rotY","sclZ" };
        bool channelSelections[5] = { true,true,true,false,false };  // �� �݊��p
    };
}
