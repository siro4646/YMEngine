#include "testScene.h"
#include "renderer/renderer.h"

#include "Game/Object/TestObject.h"
#include "Game/Object/TestObject2.h"
#include "Game/Object/TestObject3.h"
#include "Game/Object/TestObject4.h"
#include "Game/Object/player/player.h"
#include "gameFrameWork/light/pointLight.h"


#include "gameFrameWork/requiredObject/mainCamera.h"
#include "gameFrameWork/requiredObject/sphereMap/sphereMap.h"


#include "octree/octree.h"

#include "gameFrameWork/requiredObject/sceneOcTree/sceneOcTree.h"

#include "Game/Object/board/gameBoard.h"


#include "gameFrameWork/sound/soundManager.h"

#include "Game/Component/othello/gameManager.h"


namespace ym
{
	void TestScene::Init()
	{
		BaseScene::Init();

		ym::ConsoleLog("TestScene::Init()\n");
		_renderer = Renderer::Instance();

		_mainCamera = std::make_shared<MainCamera>();
		gameObjectManager_->AddGameObject(_mainCamera);

		auto sceneOcTree = gameObjectManager_->AddGameObject(std::make_shared<SceneOcTree>());

		gameObjectManager_->AddGameObject(std::make_shared<SphereMap>());



		gameObjectManager_->AddGameObject(std::make_shared<TestObject>());

		//gameObjectManager_->AddGameObject(std::make_shared<Koma>());



		/*auto gameBoard = gameObjectManager_->AddGameObject(std::make_shared<GameBoard>());
		gameBoard->AddComponent<GameManager>();

		auto soundManager = SoundManager::GetInstance();

		soundManager->LoadMP3("putSE", "asset/sound/put.mp3");*/

		//soundManager->StopBGM2D("")

		//soundManager->PlayBGM2D("gameBGM");


		//_testObject = std::make_shared<TestObject>();

		////_testObject2 = std::make_shared<TestObject2>();
		////gameObjectManager_->AddGameObject(_testObject2);

		//for (int i = 0; i < 1; i++)
		//{
		//	gameObjectManager_->AddGameObject(std::make_shared<TestObject2>());
		//	gameObjectManager_->AddGameObject(std::make_shared<TestObject3>());
		//	//gameObjectManager_->AddGameObject(std::make_shared<TestObject4>());
		//}
		//////pointLight = std::make_shared<PointLight>();
		//////auto light1 = gameObjectManager_->AddGameObject(std::make_shared<PointLight>());
		////
		//////auto light2 = gameObjectManager_->AddGameObject(std::make_shared<PointLight>());
		////for (int i = 0; i < 4; i++)
		////{
		////	gameObjectManager_->AddGameObject(std::make_shared<PointLight>());
		////}
		//gameObjectManager_->AddGameObject(std::make_shared<Player>());
		////auto gameObjectManager_->AddGameObject(std::make_shared<Player>());


		////auto warpGate1 = gameObjectManager_->AddGameObject(std::make_shared<WarpGate>());
		//////warpGate1->localTransform.Position.y = 2.0f;
		////warpGate1->localTransform.Position.z = 2.0f;

		////auto warpGate2 = gameObjectManager_->AddGameObject(std::make_shared<WarpGate>());
		////warpGate2->localTransform.Position.z = -2.0f;
		////warpGate2->localTransform.Rotation.y = 180.0f;

		//gameObjectManager_->AddGameObject(std::make_shared<WarpGateManager>());



		////auto warpgates = gameObjectManager_->FindGameObjects<WarpGate>();		
		////_testObject->Init();

		//

		////_mainCamera->Init();
	}
	void TestScene::UnInit()
	{
		BaseScene::UnInit();
		ym::ConsoleLog("TestScene::UnInit()\n");

		//_testObject->Uninit();
		//_mainCamera->Uninit();
	}
	void TestScene::FixedUpdate()
	{
		BaseScene::FixedUpdate();
		//_testObject->FixedUpdate();
		//_mainCamera->FixedUpdate();
	}
	void TestScene::Update()
	{
		static int c = 0;
		static Timer t;
		_renderer->Update();
		if (t.elapsedTime() > 2)
		{
			t.reset();
			if (c < 1000)
			{
				gameObjectManager_->AddGameObject(std::make_shared<PointLight>());
				c++;
			}
		}

		BaseScene::Update();
		//_testObject->Update();
		//_mainCamera->Update();
	}
	void TestScene::Draw()
	{
		_renderer->BeginFrame();
		BaseScene::Draw();
		//_testObject->Draw();
		//_mainCamera->Draw();
		_renderer->Draw();
		_renderer->EndFrame();
	}
}
