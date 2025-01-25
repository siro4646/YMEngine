#include "TestObject2.h"
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


namespace ym
{
	void TestObject2::Init()
	{
		name = "TestObject2";
		localTransform.Scale = { 1.0f,1.0f,1.0f };
		localTransform.Scale *= 3;
		//localTransform.Rotation = { 0.0f,180.0f,0.0f };
		float randX = rand() % 10;
		float randZ = rand() % 10;
		//localTransform.Position = { 0.0f,5.0f,0.0f };
		localTransform.Position = { randX,5.0f,randZ };


		const wchar_t *modelFile = L"asset/Alicia/FBX/Alicia_solid_Unity.FBX";
		//const wchar_t *modelFile2 = L"asset/Dragon/Dragon 2.5_fbx.fbx";
		//const wchar_t *modelFile3 = L"asset/a.fbx";
		const wchar_t *modelFile4 = L"asset/sponza (1)/sponza.obj";	
		const wchar_t *modelFile5 = L"asset/model/sphere_smooth.obj";

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

		auto boxCollider = AddComponent<SphereCollider>();
		//boxCollider->isTrigger = true;
		auto rb = AddComponent<Rigidbody>();
		//rb->useGravity = false;
	}

	void TestObject2::FixedUpdate()
	{
		Object::FixedUpdate();
		//localTransform.Position += -localTransform.GetForward() * 0.01;
	}

	void TestObject2::Update()
	{

		auto &input = KeyboardInput::GetInstance();

		
		Object::Update();
	}

	void TestObject2::Draw()
	{
		Object::Draw();
	}

	void TestObject2::Uninit()
	{
		meshes.clear();
		materials.clear();
		materials2.clear();
		materials3.clear();
		Object::Uninit();	
	}

	std::shared_ptr<Object> TestObject2::Clone()
	{
		auto copy = std::make_shared<TestObject2>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}




}

