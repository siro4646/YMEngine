#include "pointLight.h"

#include "gameFrameWork/gameObject/gameObject.h"
#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"

#include "gameFrameWork/light/lightManager.h"


namespace ym
{
	void PointLight::Init()
	{		
		//x,z座標を-3,3の間でランダムに設定
		randX = (float)(rand() % 600 - 300) /100;
		randZ = (float)(rand() % 600 - 300)/100;
		localTransform.Position = { randX, 1.0f, randZ };

		localTransform.Scale = { 1.0f,1.0f,1.0f };
		localTransform.Scale *= 4.5;
		//Object::Init();		
		objLoader = AddComponent<OBJLoader>().get();
		const wchar_t *modelFile = L"asset/model/sphere_smooth.obj";
		int flag = 0;
		flag |= ModelSetting::InverseV;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile,
			flag
		};
		meshes = objLoader->Load(importSetting);
		int count = 0;
		for (auto &mesh : meshes)
		{
			auto debugMaterial = std::make_shared<DebugMaterial>();
			materials.push_back(debugMaterial);
			objLoader->SetMaterial(debugMaterial, count);
			count++;
		}

		SetLightData();

		auto lightManager = LightManager::Instance();

		factoryIndex = lightManager->AddPointLight(data);
	}

	void PointLight::FixedUpdate()
	{

		Object::FixedUpdate();
		
	}

	void PointLight::Update()
	{
		
		angle += 0.01f;
		localTransform.Position.x = 5.0f * cos(angle) + randX;
		localTransform.Position.z = 5.0f * sin(angle) + randZ;

		Object::Update();
		SetLightData();
		auto lightManager = LightManager::Instance();
		lightManager->SetPointLight(factoryIndex, data);
	}

	void PointLight::Draw()
	{
		//Object::Draw();
	}

	void PointLight::Uninit()
	{
		auto lightManager = LightManager::Instance();
		lightManager->RemovePointLight(factoryIndex);
		materials.clear();
		meshes.clear();
		objLoader = nullptr;
		Object::Uninit();
	}
	std::shared_ptr<Object> PointLight::Clone()
	{
		auto copy = std::make_shared<PointLight>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
	void PointLight::SetLightData()
	{
		data.position.x = worldTransform.Position.ToXMFLOAT3().x;
		data.position.y = worldTransform.Position.ToXMFLOAT3().y;
		data.position.z = worldTransform.Position.ToXMFLOAT3().z;
		data.intensity = 50.0f;
		//static float randR = (rand() % 100) / 100.0f;
		//static float randG = (rand() % 100) / 100.0f;
		//float randB = (rand() % 100) / 100.0f;

		data.color.x = 1.0f;
		data.color.y = 1.0f;
		data.color.z = 1.0f;
		data.radius = worldTransform.Scale.x*0.5f;
	}
}