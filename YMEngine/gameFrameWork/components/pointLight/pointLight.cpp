#include "pointLight.h"
#include "light/lightManager.h"
#include "application/application.h"
#include "gameFrameWork/gameObject/gameObject.h"


namespace ym
{

	void PointLightComponent::Init()
	{
		r = GlobalRandom.GetFValueRange(0, 1);
		g = GlobalRandom.GetFValueRange(0, 1);
		b = GlobalRandom.GetFValueRange(0, 1);
		strength = GlobalRandom.GetFValueRange(10, 20);

		auto &transform = object->localTransform;
		transform.Scale = { 1.0f, 1.0f, 1.0f };			
		data_.intensity = strength;
		data_.color = { r, g, b };
		data_.radius = transform.Scale.x * 0.5f;

		SetLightData();

		factoryIndex_ = LightManager::Instance()->AddPointLight(data_);
		ConsoleLogRelease("PointLightComponent::Init factoryIndex: %d", factoryIndex_);
	}

	void PointLightComponent::FixedUpdate()
	{
	}

	void PointLightComponent::Update()
	{
		SetLightData();
		LightManager::Instance()->SetPointLight(factoryIndex_, data_);
	}

	void PointLightComponent::Draw()
	{
	}

	//------------------------------------------------------------
	// PointLightComponent - ImGui editable properties
	//------------------------------------------------------------
	void PointLightComponent::DrawImguiBody()
	{
		bool dirty = false;                 // ’l‚ª•Ï‚í‚Á‚½‚©‚Ç‚¤‚©”»’è
		auto &transform = object->localTransform;

		//‡@ƒfƒoƒbƒOƒ‚[ƒh
		if (ImGui::Checkbox("Debug Mode", &debugMode_))
		{	
		}
		if (debugMode_)
		{
			Transform t;
			t.Position = transform.Position;
			t.Scale = { data_.radius, data_.radius, data_.radius }; // ”¼Œa‚ðƒXƒP[ƒ‹‚ÉÝ’è
			t.Rotation = { 0.0f, 0.0f, 0.0f }; // ‰ñ“]‚Í•s—v
			debug::DebugDraw::Instance().WireSphere(t);
		}

		// ‡A F
		float col[3] = { data_.color.x, data_.color.y, data_.color.z };
		if (ImGui::ColorEdit3("Color", col))
		{
			data_.color = { col[0], col[1], col[2] };
			dirty = true;
		}

		// ‡B ‹­“x
		if (ImGui::DragFloat("Intensity", &data_.intensity, 0.1f, 0.0f, 100.0f))
		{
			dirty = true;
		}

		// ‡C ”¼Œa
		if (ImGui::DragFloat("Radius", &data_.radius, 0.05f, 0.0f, 50.0f))
		{
			dirty = true;
		}

		// ‡D ’l‚ª•Ï‚í‚Á‚½‚ç LightManager ‚É‘¦Žž”½‰f
		if (dirty)
		{
			SetLightData();                                    // ˆÊ’u‚ðXV
			LightManager::Instance()->SetPointLight(factoryIndex_, data_);
		}
	}


	void PointLightComponent::Uninit()
	{
		DebugLog("PointLightComponent::Uninit factoryIndex: %d", factoryIndex_);
		LightManager::Instance()->RemovePointLight(factoryIndex_);
	}

	void PointLightComponent::SetLightData()
	{
		const auto &pos = object->localTransform.Position;
		data_.position = pos.ToXMFLOAT3();
	}
}
