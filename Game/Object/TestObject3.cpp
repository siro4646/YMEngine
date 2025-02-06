#include "TestObject3.h"
#include "gameFrameWork/components/sprite/sprite.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "gameFrameWork/components/objLoader/objLoader.h"
#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"

#include "gameFrameWork/material/fbxMaterial.h"
#include "gameFrameWork/material/dissolveMaterial.h"
#include "gameFrameWork/material/pbrMaterial.h"

#include "gameFrameWork/collider/collider.h"

#include "gameFrameWork/components/rigidBody/rigidBody.h"
#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"



namespace ym
{
	void TestObject3::Init()
	{
		name = "TestObject3";
		localTransform.Scale = { 1.0f,1.0f,1.0f };
		localTransform.Scale *= 3;
		//localTransform.Rotation = { 0.0f,180.0f,0.0f };
		float randX = rand() % 10 - 10;
		float randZ = rand() % 10 - 10;
		//localTransform.Position = { 0.0f,5.0f,0.0f };
		localTransform.Position = { randX,5.0f,randZ };
		//ランダムにy軸回転
		localTransform.Rotation.y = rand() % 8 *45;

		const wchar_t *modelFile5 = L"asset/model/cube.obj";

		////material->Init();
		//
		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile5,
			flag
		};

		objLoader = AddComponent<OBJLoader>().get();
		meshes = objLoader->Load(importSetting);

		////fbxMaterial = std::make_shared<FBXMaterial>();

		////objLoader->SetMaterial(fbxMaterial);

		///*fbxLoader = AddComponent<FBXLoader>().get();
		//meshes= fbxLoader->Load(importSetting);*/

		for (auto &mesh : meshes)
		{
			auto fbxMaterial = std::make_shared<FBXMaterial>();		
			materials.push_back(fbxMaterial);
			auto dissolveMaterial = std::make_shared<DissolveMaterial>();
			materials2.push_back(dissolveMaterial);
			auto pbrMaterial = std::make_shared<PBRMaterial>();
			materials3.push_back(pbrMaterial);
		}	
		
		int count = 0;
		for (auto material : materials3)
		{
			objLoader->SetMaterial(material,count);
			count++;
		}

		//localTransform.Position = { 0.0f,0.0f,0.0f };

		/*auto sprite = AddComponent<ym::Sprite>();
		sprite->LoadTexture("asset/texture/Jacket_BaseColor.jpg");*/

		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		sceneRenderRegistrar->AddRenderObject(this);	

		auto boxCollider = AddComponent<BoxCollider>();
		auto rb = AddComponent<Rigidbody>().get();
		rb->mass = 1000;
	}

	void TestObject3::FixedUpdate()
	{
		Object::FixedUpdate();
		//localTransform.Position += -localTransform.GetForward() * 0.01;
	}

	void TestObject3::Update()
	{

		auto &input = KeyboardInput::GetInstance();
		if (input.GetKeyDown("7"))
		{
			auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
			sceneRenderRegistrar->AddRenderObject(this);
		}
		if (input.GetKeyDown("8"))
		{
			auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
			sceneRenderRegistrar->RemoveRenderObject(this);
		}

		
		Object::Update();
	}

	void TestObject3::Draw()
	{
		Object::Draw();
	}

	void TestObject3::Uninit()
	{
		meshes.clear();
		materials.clear();
		materials2.clear();
		materials3.clear();
		Object::Uninit();	
	}

	std::shared_ptr<Object> TestObject3::Clone()
	{
		auto copy = std::make_shared<TestObject3>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}




}

