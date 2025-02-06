#include "pointLight.h"

#include "gameFrameWork/gameObject/gameObject.h"
#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"

#include "gameFrameWork/light/lightManager.h"

#include "application/application.h"

namespace ym
{
	void PointLight::Init()
	{		
		//x,z座標を-3,3の間でランダムに設定
		randX = (float)(rand() % 600 - 300) /100 ;
		randZ = (float)(rand() % 1200 - 600)/100;

		r = (rand() % 2);
		g = (rand() % 2);
		b = (rand() % 2);

		localTransform.Position = { randX, 0.0f, randZ };

		localTransform.Scale = { 1.0f,1.0f,1.0f };
		localTransform.Scale *= 10;
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
		float deltaTime = Application::Instance()->GetDeltaTime();		

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
		data.intensity = 10.0f;
		//static float randR = (rand() % 100) / 100.0f;
		//static float randG = (rand() % 100) / 100.0f;
		//float randB = (rand() % 100) / 100.0f;

		data.color.x = r;
		data.color.y = g;
		data.color.z = b;
		data.radius = worldTransform.Scale.x*0.5f;
	}
}