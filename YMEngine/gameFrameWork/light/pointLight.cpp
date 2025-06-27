//#include "pointLight.h"
//
//#include "gameFrameWork/gameObject/gameObject.h"
//#include "gameFrameWork/components/objLoader/objLoader.h"
//
//#include "gameFrameWork/material/debugMaterial.h"
//
//#include "gameFrameWork/light/lightManager.h"
//
//#include "application/application.h"
//
//namespace ym
//{
//	void PointLight::Init()
//	{
//		r = ym::GlobalRandom.GetFValueRange(0, 1);
//		g = ym::GlobalRandom.GetFValueRange(0, 1);
//		b = ym::GlobalRandom.GetFValueRange(0, 1);
//
//		strength = ym::GlobalRandom.GetFValueRange(10, 20);
//
//		localTransform.Scale = { 1.0f,1.0f,1.0f };
//		localTransform.Scale *= 1;
//		//Object::Init();		
//		objLoader = AddComponent<OBJLoader>().get();
//		const wchar_t *modelFile = L"asset/model/sphere_smooth.obj";
//		int flag = 0;
//		flag |= ModelSetting::InverseV;
//		flag |= ModelSetting::AdjustCenter;
//		flag |= ModelSetting::AdjustScale;
//		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
//		{
//			modelFile,
//			flag
//		};
//		meshes = objLoader->Load(importSetting);
//		int count = 0;
//		for (auto &mesh : meshes)
//		{
//			auto debugMaterial = std::make_shared<DebugMaterial>();
//			materials.push_back(debugMaterial);
//			objLoader->SetMaterial(debugMaterial, count);
//			count++;
//		}
//		data.intensity = strength;
//		data.color.x = r;
//		data.color.y = g;
//		data.color.z = b;
//		data.radius = localTransform.Scale.x * 0.5f;
//
//		SetLightData();
//
//		auto lightManager = LightManager::Instance();
//
//		factoryIndex = lightManager->AddPointLight(data);
//		ym::ConsoleLogRelease("PointLight::Init factoryIndex: %d", factoryIndex);
//	}
//
//	void PointLight::FixedUpdate()
//	{
//
//		Object::FixedUpdate();
//		
//	}
//
//	void PointLight::Update()
//	{
//		float deltaTime = Application::Instance()->GetDeltaTime();		
//
//		Object::Update();
//		SetLightData();
//		auto lightManager = LightManager::Instance();
//		lightManager->SetPointLight(factoryIndex, data);
//	}
//
//	void PointLight::Draw()
//	{
//		//Object::Draw();
//	}
//
//	void PointLight::Uninit()
//	{
//		auto lightManager = LightManager::Instance();
//		lightManager->RemovePointLight(factoryIndex);
//		materials.clear();
//		meshes.clear();
//		objLoader = nullptr;
//		Object::Uninit();
//	}
//	std::shared_ptr<Object> PointLight::Clone()
//	{
//		auto copy = std::make_shared<PointLight>(*this); // コピーコンストラクタを使用
//		// 子オブジェクトは深いコピーを行う
//		for (const auto &child : _childs) {
//			copy->_childs.push_back(child->Clone());
//		}
//		return copy;
//	}
//	void PointLight::SetLightData()
//	{
//		data.position.x = localTransform.Position.ToXMFLOAT3().x;
//		data.position.y = localTransform.Position.ToXMFLOAT3().y;
//		data.position.z = localTransform.Position.ToXMFLOAT3().z;
//		ym::ConsoleLogRelease("PointLight::SetLightData position: %f %f %f\n", data.position.x, data.position.y, data.position.z);
//		//static float randR = (rand() % 100) / 100.0f;
//		//static float randG = (rand() % 100) / 100.0f;
//		//float randB = (rand() % 100) / 100.0f;
//
//		//data.color.x = r;
//		//data.color.y = g;
//		//data.color.z = b;
//		//data.radius = worldTransform.Scale.x*0.5f;
//	}
//}