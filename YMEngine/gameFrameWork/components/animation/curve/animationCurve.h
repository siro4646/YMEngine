#pragma once
namespace ym
{
    class AnimationCurve {
    public:
        // 時間と値のペア（例：0.0秒で0.0、1.0秒で10.0）
        std::vector<std::pair<float, float>> keyframes;

        // 時間に対する補間された値を返す
        float Evaluate(float time) const;

        // キーフレーム追加
        void AddKey(float time, float value);
		// キーフレーム削除
		bool RemoveKey(float time);

		bool HasKey(float time) const;

		// キーフレームの数を取得
		size_t GetKeyCount() const { return keyframes.size(); }

		// キーフレームの時刻を取得
		float GetKeyTime(size_t index) const { return keyframes[index].first; }

		// キーフレームの値を取得
		float GetKeyValue(size_t index) const { return keyframes[index].second; }

		// キーフレームの時刻を設定
		void SetKeyTime(size_t index, float time) { keyframes[index].first = time; }

		// キーフレームの値を設定
		void SetKeyValue(size_t index, float value) { keyframes[index].second = value; }
    };

}