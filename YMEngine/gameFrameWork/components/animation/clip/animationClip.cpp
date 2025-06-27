#include "animationClip.h"
#include <fstream>

using json = nlohmann::json;

namespace ym
{
	const std::unordered_map<std::string, AnimationCurve> &AnimationClip::GetAllCurves() const
	{
		return curves_;
	}

	AnimationCurve *AnimationClip::GetCurveForProperty(const std::string &propertyName)
	{
		auto it = curves_.find(propertyName);
		if (it != curves_.end()) {
			return &it->second;
		}
		return nullptr;
	}

	AnimationCurve &AnimationClip::AddCurveForProperty(const std::string &propertyName)
	{
		DebugLog("Adding curve for property: %s", propertyName.c_str());
		return curves_[propertyName]; // ë∂ç›ÇµÇ»Ç¢Ç»ÇÁêVãKçÏê¨
	}

	void AnimationClip::SaveToJsonFile(const std::string &path) const
	{
		json j;
		j["length"] = length;

		for (const auto &[name, curve] : curves_) {
			json keyArray = json::array();
			for (const auto &[time, value] : curve.keyframes) {
				keyArray.push_back({ time, value });
			}
			j["curves"][name] = keyArray;
		}

		std::ofstream ofs(path);
		if (ofs) {
			ofs << j.dump(4);
		}
	}

	std::shared_ptr<AnimationClip> AnimationClip::LoadFromJsonFile(const std::string &path)
	{
		std::ifstream ifs(path);
		if (!ifs) return nullptr;

		json j;
		ifs >> j;

		auto clip = std::make_shared<AnimationClip>();
		clip->length = j["length"].get<float>();

		for (auto &[name, keyArray] : j["curves"].items()) {
			auto &curve = clip->curves_[name];
			for (const auto &key : keyArray) {
				float t = key[0].get<float>();
				float v = key[1].get<float>();
				curve.AddKey(t, v);
			}
		}

		return clip;
	}
}
