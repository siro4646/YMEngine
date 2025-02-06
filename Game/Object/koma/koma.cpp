#include "Koma.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"

#include "gameFrameWork/requiredObject/mainCamera.h"


#include "gameFrameWork/components/objLoader/objLoader.h"
#include "gameFrameWork/material/material.h"

#include "gameFrameWork/sound/soundManager.h"

namespace ym
{
	void Koma::Init()
	{
		name = "Koma";

		auto objLoader = AddComponent<OBJLoader>().get();
		auto importSetting = ImportSettings
		{
			L"asset/othello/koma.fbx",
			ModelSetting::InverseV | ModelSetting::AdjustCenter | ModelSetting::AdjustScale
		};
		auto meshes = objLoader->Load(importSetting);
		meshes[0].DiffuseMap = L"black";
		objLoader->material_[0]->CreateMaterial(meshes[0]);
		int c = 0;
		localTransform.Position.y = 1.5f;
		//for (auto &mesh : meshes)
		//{
		//	//mesh.DiffuseMap = L"black";
		//	objLoader->material_[c]->CreateMaterial(mesh);
		//	c++;
		//	//mesh
		//}



	}

	void Koma::FixedUpdate()
	{
		
		Object::FixedUpdate();
		if (_komaState == KomaState::Flip)
		{
			//ym::ConsoleLogRelease("%f %f\n", timer.elapsedTime(),_flipStartTimer);
			if (timer.elapsedTime() > _flipStartTimer) 
			{

				if (localTransform.Position.y < 1.5)
				{
					localTransform.Position.y = 1.5;
					_komaState = KomaState::Normal;

					auto soundManager = SoundManager::GetInstance();
					soundManager->PlaySE2D("putSE");

				}
				else
				{
					localTransform.Position.y -= 1;
				}
			}
		}
		else
		{
			//ym::ConsoleLogRelease("reset\n");
			timer.reset();
		}

	}

	void Koma::Update()
	{
		//localTransform.Rotation.x += 0.1f;


		Object::Update();

	}

	void Koma::Draw()
	{
		Object::Draw();

	}

	void Koma::Uninit()
	{


		Object::Uninit();

	}

	std::shared_ptr<Object> Koma::Clone()
	{
		auto copy = std::make_shared<Koma>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}


}

