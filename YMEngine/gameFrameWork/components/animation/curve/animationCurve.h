#pragma once
namespace ym
{
    class AnimationCurve {
    public:
        // ���Ԃƒl�̃y�A�i��F0.0�b��0.0�A1.0�b��10.0�j
        std::vector<std::pair<float, float>> keyframes;

        // ���Ԃɑ΂����Ԃ��ꂽ�l��Ԃ�
        float Evaluate(float time) const;

        // �L�[�t���[���ǉ�
        void AddKey(float time, float value);
		// �L�[�t���[���폜
		bool RemoveKey(float time);

		bool HasKey(float time) const;

		// �L�[�t���[���̐����擾
		size_t GetKeyCount() const { return keyframes.size(); }

		// �L�[�t���[���̎������擾
		float GetKeyTime(size_t index) const { return keyframes[index].first; }

		// �L�[�t���[���̒l���擾
		float GetKeyValue(size_t index) const { return keyframes[index].second; }

		// �L�[�t���[���̎�����ݒ�
		void SetKeyTime(size_t index, float time) { keyframes[index].first = time; }

		// �L�[�t���[���̒l��ݒ�
		void SetKeyValue(size_t index, float value) { keyframes[index].second = value; }
    };

}