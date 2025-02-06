#include "TestObject4.h"
#include "gameFrameWork/components/sprite/sprite.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "gameFrameWork/components/objLoader/objLoader.h"
#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"

#include "gameFrameWork/material/fbxMaterial.h"
#include "gameFrameWork/material/dissolveMaterial.h"
#include "gameFrameWork/material/pbrMaterial.h"
#include "gameFrameWork/material/tornadoMaterial.h"

#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"

#include "gameFrameWork/collider/collider.h"

namespace ym
{
	void TestObject4::Init()
	{
		name = "TestObject4";
		//localTransform.Scale = { 3.0f,0.01f,3.0f };
		localTransform.Scale *= 4;
		//localTransform.Rotation = { 0.0f,0.0f,0.0f };
		//localTransform.Position = { 0.0f,-0.5f,1.2f };

		float randX = rand() % 5*5;
		float randZ = rand() % 5*5;
		//localTransform.Position = { 0.0f,5.0f,0.0f };
		localTransform.Position = { randX,2.0f,randZ };


		const wchar_t *modelFile8 = L"asset/model/tornado.fbx";




		//material->Init();
		
		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile8,
			flag
		};

		/*objLoader = AddComponent<OBJLoader>().get();
		meshes = objLoader->Load(importSetting);*/
		//fbxMaterial = std::make_shared<FBXMaterial>();

		//objLoader->SetMaterial(fbxMaterial);

		fbxLoader = AddComponent<FBXLoader>().get();
		meshes= fbxLoader->Load(importSetting);

		for (auto &mesh : meshes)
		{
			auto fbxMaterial = std::make_shared<FBXMaterial>();		
			materials.push_back(fbxMaterial);
			auto dissolveMaterial = std::make_shared<DissolveMaterial>();
			materials2.push_back(dissolveMaterial);
			auto pbrMaterial = std::make_shared<PBRMaterial>();
			materials3.push_back(pbrMaterial);
			auto tornadoMaterial = std::make_shared<TornadoMaterial>();
			materials4.push_back(tornadoMaterial);
		}	
		int count = 0;
		for (auto &mesh : meshes)
		{
			fbxLoader->SetMaterial(materials4[count], count);
			count++;
		}

		//localTransform.Position = { 0.0f,0.0f,0.0f };

		/*auto sprite = AddComponent<ym::Sprite>();
		sprite->LoadTexture("asset/texture/Jacket_BaseColor.jpg");*/
		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		sceneRenderRegistrar->AddRenderObject(this);

		//auto boxCollider = AddComponent<BoxCollider>().get();
		//auto boxCollider = AddComponent<SphereCollider>().get();

	}

	void TestObject4::FixedUpdate()
	{
		Object::FixedUpdate();

		
	}

	void TestObject4::Update()
	{

		Object::Update();

	}

	void TestObject4::Draw()
	{
		Object::Draw();

	}

	void TestObject4::Uninit()
	{

		materials.clear();
		materials2.clear();
		materials3.clear();
		meshes.clear();

		Object::Uninit();

	}

	std::shared_ptr<Object> TestObject4::Clone()
	{
		auto copy = std::make_shared<TestObject4>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}


}

