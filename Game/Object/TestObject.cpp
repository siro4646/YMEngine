#include "TestObject.h"
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
	void TestObject::Init()
	{
		name = "TestObject";
		localTransform.Scale = { 3.0f,0.01f,3.0f };
		//localTransform.Scale *= 10;

		localTransform.Scale *= 100;
		//localTransform.Rotation = { 0.0f,0.0f,0.0f };
		//localTransform.Position = { 0.0f,-0.5f,1.2f };

		localTransform.Position = { 0.0f,-1.0f,2.0f };

		const wchar_t *modelFile = L"asset/Alicia/FBX/Alicia_solid_Unity.FBX";
		const wchar_t *modelFile2 = L"asset/Dragon/Dragon 2.5_fbx.fbx";
		const wchar_t *modelFile3 = L"asset/a.fbx";
		const wchar_t *modelFile4 = L"asset/sponza (1)/sponza.obj";	
		const wchar_t *modelFile5 = L"asset/model/sphere_smooth.obj";
		const wchar_t *modelFile6 = L"asset/model/cylinder.obj";
		const wchar_t *modelFile7 = L"asset/model/cube.obj";
		const wchar_t *modelFile8 = L"asset/model/tornado.fbx";




		//material->Init();
		
		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile7,
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
			fbxLoader->SetMaterial(materials3[count], count);
			count++;
		}

		//localTransform.Position = { 0.0f,0.0f,0.0f };

		/*auto sprite = AddComponent<ym::Sprite>();
		sprite->LoadTexture("asset/texture/Jacket_BaseColor.jpg");*/
		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		sceneRenderRegistrar->AddRenderObject(this);

		auto boxCollider = AddComponent<BoxCollider>().get();
		//auto boxCollider = AddComponent<SphereCollider>().get();

	}

	void TestObject::FixedUpdate()
	{
		Object::FixedUpdate();

		
	}

	void TestObject::Update()
	{
		auto &input = KeyboardInput::GetInstance();
		static float speed = 0.0f;
		static bool flag = false;
		//if (!flag)
		//{
		//	speed += 0.01f;			
		//	//materials2[0]->SetTime(sin(speed));
			for (auto &material : materials2)
			{
				material->SetTime(sin(dissolve));
			}
			//dissolve += 0.01f;

		//}




		/*if (input.GetKey("LEFT"))
		{
			localTransform.Rotation.y -= 1.0f;
		}
		if (input.GetKey("RIGHT"))
		{
			localTransform.Rotation.y += 1.0f;
		}*/
		if (input.GetKeyDown("UP"))
		{		
			int count = 0;
			ym::ConsoleLog("PBRマテリアル\n");
			for (auto &mesh : meshes)
			{
				fbxLoader->SetMaterial(materials3[count], count);
				count++;
			}
			flag = true;
		}
		if (input.GetKeyDown("DOWN"))
		{
			int count = 0;
			ym::ConsoleLog("Dissolve\n");
			for (auto &mesh : meshes)
			{
				fbxLoader->SetMaterial(materials2[count], count);
				count++;
			}
			flag = false;
		}

	


		Object::Update();

	}

	void TestObject::Draw()
	{
		Object::Draw();

	}

	void TestObject::Uninit()
	{

		materials.clear();
		materials2.clear();
		materials3.clear();
		meshes.clear();

		Object::Uninit();

	}

	std::shared_ptr<Object> TestObject::Clone()
	{
		auto copy = std::make_shared<TestObject>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}


}

