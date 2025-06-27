#pragma once

#include "../component.h"

namespace ym
{

	struct PointLightData {
		DirectX::XMFLOAT3 position;
		float padding1;
		DirectX::XMFLOAT3 color;
		float padding2;
		float intensity;
		float radius;
		float padding3[2];
	};

	class PointLightComponent : public Component
	{
	public:
		using Component::Component;

		void Init() override;
		void FixedUpdate() override;
		void Update() override;
		void Draw() override;
		void DrawImguiBody() override;
		void Uninit() override;
		const char *GetName() const override { return "PointLightComponent"; }
		PointLightData &GetData() { return data_; }

	private:
		void SetLightData();

		float r = 0, g = 0, b = 0;
		float strength = 0;

		PointLightData data_;
		int factoryIndex_ = -1;

		bool debugMode_ = false; // デバッグモードのフラグ
	};
	REGISTER_COMPONENT(PointLightComponent);
}
