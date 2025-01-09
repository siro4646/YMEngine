#include "TestObject.h"
#include "gameFrameWork/components/sprite/sprite.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "gameFrameWork/components/objLoader/objLoader.h"
#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"

#include "gameFrameWork/material/fbxMaterial.h"


namespace ym
{
	void TestObject::Init()
	{
		name = "TestObject";
		globalTransform.Scale = { 1.0f,1.0f,1.0f };
		globalTransform.Scale *= 5;
		globalTransform.Rotation = { 0.0f,90.0f,0.0f };
		globalTransform.Position = { 0.0f,0.0f,0.0f };

		const wchar_t *modelFile = L"asset/Alicia/FBX/Alicia_solid_Unity.FBX";
		const wchar_t *modelFile2 = L"asset/Dragon/Dragon 2.5_fbx.fbx";
		const wchar_t *modelFile3 = L"asset/a.fbx";
		const wchar_t *modelFile4 = L"asset/sponza (1)/sponza.obj";		
		//material->Init();
		
		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile4,
			flag
		};

		objLoader = AddComponent<OBJLoader>().get();
		meshes = objLoader->Load(importSetting);
		for (auto &mesh : meshes)
		{
			auto fbxMaterial = std::make_shared<FBXMaterial>();
			//fbxMaterial->SetMesh(mesh);
			materials.push_back(fbxMaterial);
		}		
		//fbxMaterial = std::make_shared<FBXMaterial>();

		//objLoader->SetMaterial(fbxMaterial);

		/*auto fbxLoader = AddComponent<FBXLoader>();
		fbxLoader->Load(importSetting);*/

		//globalTransform.Position = { 0.0f,0.0f,0.0f };

		/*auto sprite = AddComponent<ym::Sprite>();
		sprite->LoadTexture("asset/texture/Jacket_BaseColor.jpg");*/
	}

	void TestObject::FixedUpdate()
	{
		
	}

	void TestObject::Update()
	{
		auto &input = KeyboardInput::GetInstance();
		if (input.GetKey("LEFT"))
		{
			globalTransform.Rotation.y -= 1.0f;
		}
		if (input.GetKey("RIGHT"))
		{
			globalTransform.Rotation.y += 1.0f;
		}
		if (input.GetKeyDown("UP"))
		{		
			int count = 0;
			for (auto &mesh : meshes)
			{
				objLoader->SetMaterial(materials[count], count);
				count++;
			}
		}
		if (input.GetKeyDown("DOWN"))
		{
			int count = 0;
			for (auto &mesh : meshes)
			{
				objLoader->SetMaterial(std::make_shared<Material>(), count);
				count++;
			}
		}


		Object::Update();

	}

	void TestObject::Draw()
	{
		Object::Draw();
	}

	void TestObject::Uninit()
	{


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

