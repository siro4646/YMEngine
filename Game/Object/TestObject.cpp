#include "TestObject.h"
#include "gameFrameWork/components/sprite/sprite.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"


namespace ym
{
	void TestObject::Init()
	{
		globalTransform.Scale = { 1.0f,1.0f,1.0f };
		globalTransform.Scale *= 1;
		globalTransform.Rotation = { 0.0f,180.0f,0.0f };
		globalTransform.Position = { 0.0f,0.0f,0.0f };

		const wchar_t *modelFile = L"asset/Alicia/FBX/Alicia_solid_Unity.FBX";
		const wchar_t *modelFile2 = L"asset/Dragon/Dragon 2.5_fbx.fbx";
		const wchar_t *modelFile3 = L"asset/a.fbx";


		std::vector<Mesh> meshes;
		int flag = 0;
		flag |= ModelSetting::InverseV;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		auto fbxLoader = AddComponent<FBXLoader>();
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile,
			flag
		};
		fbxLoader->Load(importSetting);

		//globalTransform.Position = { 0.0f,0.0f,0.0f };

		auto sprite = AddComponent<ym::Sprite>();
		sprite->LoadTexture("asset/texture/Jacket_BaseColor.jpg");
	}

	void TestObject::FixedUpdate()
	{
		
	}

	void TestObject::Update()
	{
		auto &input = KeyboardInput::GetInstance();
		if (input.GetKey("W"))
		{
			globalTransform.Position.z += 0.05f;
		}
		if (input.GetKey("S"))
		{
			globalTransform.Position.z -= 0.05f;
		}
		if (input.GetKey("A"))
		{
			globalTransform.Position.x -= 0.05f;
		}
		if (input.GetKey("D"))
		{
			globalTransform.Position.x += 0.05f;
		}
		if (input.GetKey("Q"))
		{
			globalTransform.Position.y += 0.05f;
		}
		if (input.GetKey("E"))
		{
			globalTransform.Position.y -= 0.05f;
		}
		//サイズ変える
		if (input.GetKey("T"))
		{
			globalTransform.Rotation.y += 1.0f;
		}
		if (input.GetKey("Y"))
		{
			globalTransform.Rotation.y -= 1.0f;

		}
		if (input.GetKey("0"))
		{
			globalTransform.Position = { 0.0f,0.0f,0.0f };
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

