#include "sphereMap.h"

#include "gameFrameWork/gameObject/gameObject.h"
#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"

#include "gameFrameWork/light/lightManager.h"

#include "gameFrameWork/material/objMaterial.h"

#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"


namespace ym
{
	void SphereMap::Init()
	{
		localTransform.Position = { 0.0f,0.0f,0.0f };
		localTransform.Scale = { 1.0f,1.0f,1.0f };
		localTransform.Scale *= 100;

		name = "SphereMap";

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
			auto debugMaterial = std::make_shared<OBJMaterial>();
			debugMaterial->SetMainTex("asset/texture/sky.dds");
			materials.push_back(debugMaterial);
			objLoader->SetMaterial(debugMaterial, count);
			count++;
		}	
		auto srr = SceneRenderRegistrar::Instance();
		srr->AddRenderObject(this);
	}

	void SphereMap::FixedUpdate()
	{

		Object::FixedUpdate();

	}

	void SphereMap::Update()
	{
		Object::Update();
	}

	void SphereMap::Draw()
	{
		Object::Draw();
	}

	void SphereMap::Uninit()
	{
		materials.clear();
		meshes.clear();
		objLoader = nullptr;
		Object::Uninit();
	}
	std::shared_ptr<Object> SphereMap::Clone()
	{
		auto copy = std::make_shared<SphereMap>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
}