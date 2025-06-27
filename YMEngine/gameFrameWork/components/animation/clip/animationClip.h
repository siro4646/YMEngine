#pragma once
#include "../curve/animationCurve.h"

namespace ym
{
	class AnimationClip
	{
	public:
		AnimationClip() = default;
		~AnimationClip() = default;

		// クリップ全体の長さ（秒）
		float length = 0.0f;

		// 全カーブ取得（読み取り用）
		const std::unordered_map<std::string, AnimationCurve> &GetAllCurves() const;

		// カーブ取得（存在しない場合は nullptr）
		AnimationCurve *GetCurveForProperty(const std::string &propertyName);

		// カーブ追加または取得（存在しない場合は新規作成して返す）
		AnimationCurve &AddCurveForProperty(const std::string &propertyName);

		// Json保存・読み込み
		void SaveToJsonFile(const std::string &path) const;
		static std::shared_ptr<AnimationClip> LoadFromJsonFile(const std::string &path);

	private:
		std::unordered_map<std::string, AnimationCurve> curves_;
	};
}
