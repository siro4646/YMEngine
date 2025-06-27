#pragma once
#include "../curve/animationCurve.h"

namespace ym
{
	class AnimationClip
	{
	public:
		AnimationClip() = default;
		~AnimationClip() = default;

		// �N���b�v�S�̂̒����i�b�j
		float length = 0.0f;

		// �S�J�[�u�擾�i�ǂݎ��p�j
		const std::unordered_map<std::string, AnimationCurve> &GetAllCurves() const;

		// �J�[�u�擾�i���݂��Ȃ��ꍇ�� nullptr�j
		AnimationCurve *GetCurveForProperty(const std::string &propertyName);

		// �J�[�u�ǉ��܂��͎擾�i���݂��Ȃ��ꍇ�͐V�K�쐬���ĕԂ��j
		AnimationCurve &AddCurveForProperty(const std::string &propertyName);

		// Json�ۑ��E�ǂݍ���
		void SaveToJsonFile(const std::string &path) const;
		static std::shared_ptr<AnimationClip> LoadFromJsonFile(const std::string &path);

	private:
		std::unordered_map<std::string, AnimationCurve> curves_;
	};
}
