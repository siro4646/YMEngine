#include "animationCurve.h"
namespace ym
{
    float AnimationCurve::Evaluate(float time) const
    {
        if (keyframes.empty()) return 0.0f;

        // time ���͈͊O�̂Ƃ��͒[�̒l��Ԃ�
        if (time <= keyframes.front().first) return keyframes.front().second;
        if (time >= keyframes.back().first) return keyframes.back().second;

        // ���`��ԁi�O��̃L�[�t���[����T���j
        for (size_t i = 0; i < keyframes.size() - 1; ++i)
        {
            const auto &[t1, v1] = keyframes[i];
            const auto &[t2, v2] = keyframes[i + 1];
            if (t1 <= time && time <= t2)
            {
                float alpha = (time - t1) / (t2 - t1);
                return v1 * (1.0f - alpha) + v2 * alpha;
            }
        }

        return 0.0f; // ���B���Ȃ��͂�
    }

    void AnimationCurve::AddKey(float time, float value)
    {
        keyframes.emplace_back(time, value);

        // ���ԏ��ɕ��ׂĂ����iEvaluate���������@�\����悤�Ɂj
        std::sort(keyframes.begin(), keyframes.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });
    }
    bool AnimationCurve::RemoveKey(float time)
    {
        auto it = std::find_if(keyframes.begin(), keyframes.end(),
            [time](const auto &kv) { return std::abs(kv.first - time) < 0.001f; });

        if (it != keyframes.end())
        {
            keyframes.erase(it);
            return true;
        }
        return false;
    }

    bool AnimationCurve::HasKey(float time) const
    {
        for (const auto &[t, v] : keyframes)
        {
            if (std::abs(t - time) < 1e-4f) // float�덷���l�����Ĕ�r
            {
                return true;
            }
        }
        return false;
    }


} // namespace ym