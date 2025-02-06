#include "ChildBoard.h"
#include "gameFrameWork/components/objLoader/objLoader.h"
#include "gameFrameWork/material/material.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"

#include "Game/Object/koma/koma.h"

#include "gameFrameWork/gameObject/gameObjectManager.h"

namespace ym
{
	void ChildBoard::Init()
	{
		name = "ChildBoard";
	
		localTransform.Scale.y = 0.01;
		// localTransform.Scale = { 0.2f,0.01f,0.2f };
		//localTransform.Position = { -1.4f,1,1.35 };

		objLoader = AddComponent<OBJLoader>().get();
		auto importSetting = ImportSettings
		{
			L"asset/model/cube.obj",
			ModelSetting::InverseV | ModelSetting::AdjustCenter | ModelSetting::AdjustScale
		};
		meshes = objLoader->Load(importSetting);
		int c = 0;
		for (auto &mesh : meshes)
		{
			mesh.DiffuseMap = L"green";
			objLoader->material_[c]->CreateMaterial(mesh);
			c++;
			//mesh
		}


	}

	void ChildBoard::FixedUpdate()
	{
		Object::FixedUpdate();


	}

	void ChildBoard::Update()
	{
		auto &key = KeyboardInput::GetInstance();


		Object::Update();

	}

	void ChildBoard::Draw()
	{
		Object::Draw();

	}

	void ChildBoard::Uninit()
	{


		Object::Uninit();

	}

	std::shared_ptr<Object> ChildBoard::Clone()
	{
		auto copy = std::make_shared<ChildBoard>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}

	void ChildBoard::PutKoma(KomaType komaType)
	{
		_komaType = komaType;
		auto koma = objectManager->AddGameObject(make_shared<Koma>());
		if (_komaType == KomaType::Black)
		{
			koma->localTransform.Rotation.x = 180;
		}
		AddChild(koma);

	}

	void ChildBoard::FlipKoma(KomaType komaType, float flipTime)
	{
		_komaType = komaType;
		auto koma = static_cast<Koma *>(_childs[0].get());
		koma->SetKomaType(_komaType);
		koma->SetKomaState(KomaState::Flip);
		koma->SetFlipStartTimer(flipTime);
		koma->localTransform.Position.y = 80;
		if (_komaType == KomaType::Black)
		{
			koma->localTransform.Rotation.x = 180;
			
		}
		else
		{
			koma->localTransform.Rotation.x = 0;
		}
	}

	void ChildBoard::SetTexture(string texturePath)
	{
		int c = 0;
		for (auto &mesh : meshes)
		{
			mesh.DiffuseMap = ym::Utf8ToUtf16(texturePath);
			objLoader->material_[c]->SetMainTex(texturePath);
			objLoader->material_[c]->CreateMaterial(mesh);
			c++;
		}
	}


}

