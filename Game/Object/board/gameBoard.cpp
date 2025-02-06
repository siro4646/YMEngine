#include "GameBoard.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"

#include "childBoard.h"

#include "gameFrameWork/requiredObject/mainCamera.h"

#include "Game/Object/koma/koma.h"

#include "Game/Component/othello/gameManager.h"

namespace ym
{
	void GameBoard::Init()
	{
		name = "GameBoard";

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				auto c = objectManager->AddGameObject(make_shared<ChildBoard>());
				auto castBoard = static_cast<ChildBoard *>(c.get());
				castBoard->localTransform.Position = { i * 1.1f - 4,0,j * 1.1f - 4};

				if (i == 3)
				{
					if (j == 3)
					{
						castBoard->PutKoma(KomaType::Black);
						//auto koma = objectManager->AddGameObject(make_shared<Koma>());
						////koma->localTransform.Position = { i * 1.1f - 4,0,j * 1.1f - 4 };
						////koma->localTransform.Position.y
						//koma->localTransform.Rotation.x = 180;
						//castBoard->AddChild(koma);
					}
					if (j == 4)
					{

						castBoard->PutKoma(KomaType::White);

						//auto koma = objectManager->AddGameObject(make_shared<Koma>());
						////koma->localTransform.Position = { i * 1.1f - 4,0,j * 1.1f - 4 };
						//castBoard->AddChild(koma);
					}

				}

				if (i == 4)
				{
					if (j == 3)
					{


						castBoard->PutKoma(KomaType::White);

						//auto koma = objectManager->AddGameObject(make_shared<Koma>());
						////koma->localTransform.Position = { i * 1.1f - 4,0,j * 1.1f - 4 };
						//castBoard->AddChild(koma);
					}
					if (j == 4)
					{

						castBoard->PutKoma(KomaType::Black);

						//auto koma = objectManager->AddGameObject(make_shared<Koma>());
						////koma->localTransform.Position = { i * 1.1f - 4,0,j * 1.1f - 4 };
						//koma->localTransform.Rotation.x = 180;
						//castBoard->AddChild(koma);
						
					}

				}
				AddChild(c);
				_childBoards.push_back(static_cast<ChildBoard*>(c.get()));
			}
		}
		/*auto c =  objectManager->AddGameObject(make_shared<ChildBoard>());

		AddChild(c);
		_childBoards.push_back(c.get());*/
		auto camera = objectManager->FindGameObjects<MainCamera>();

		camera[0]->localTransform.Position = { 0,10,-5 };
		camera[0]->localTransform.Rotation = { 60,0,0 };

		//AddComponent<GameManager>();

		AddChild(camera[0]);

	}

	void GameBoard::FixedUpdate()
	{
		Object::FixedUpdate();


	}

	void GameBoard::Update()
	{
	


		Object::Update();

	}

	void GameBoard::Draw()
	{
		Object::Draw();

	}

	void GameBoard::Uninit()
	{

		
		Object::Uninit();

	}

	std::shared_ptr<Object> GameBoard::Clone()
	{
		auto copy = std::make_shared<GameBoard>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}


}

