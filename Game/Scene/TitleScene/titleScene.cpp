#include "titleScene.h"
#include "renderer/renderer.h"

#include "gameFrameWork/requiredObject/sphereMap/sphereMap.h"


#include "octree/octree.h"

#include "gameFrameWork/requiredObject/sceneOcTree/sceneOcTree.h"

#include "Game/Object/board/gameBoard.h"

#include "Game/Object/titleSprite/titleSprite.h"

#include "gameFrameWork/sound/soundManager.h"

#include "scene/sceneManager.h"

#include "Game/Scene/TestScene/testScene.h"

#include "gameFrameWork/requiredObject/mainCamera.h"

namespace ym
{
	void TitleScene::Init()
	{
		BaseScene::Init();

		ym::ConsoleLog("TitleScene::Init()\n");
		_renderer = Renderer::Instance();

		_mainCamera = std::make_shared<MainCamera>();
		gameObjectManager_->AddGameObject(_mainCamera);

		auto sceneOcTree = gameObjectManager_->AddGameObject(std::make_shared<SceneOcTree>());

		gameObjectManager_->AddGameObject(std::make_shared<SphereMap>());
	
		auto soundManager = SoundManager::GetInstance();

		soundManager->LoadMP3("titleBGM", "asset/sound/title.mp3");

		soundManager->PlayBGM2D("titleBGM");

		gameObjectManager_->AddGameObject(std::make_shared<TitleSprite>());



	}
	void TitleScene::UnInit()
	{
		BaseScene::UnInit();
		ym::ConsoleLog("TitleScene::UnInit()\n");


		auto soundManager = SoundManager::GetInstance();

		soundManager->AllStopBGM2D();

		//soundManager->StopBGM2D("titleBGM");

		//_testObject->Uninit();
		//_mainCamera->Uninit();
	}
	void TitleScene::FixedUpdate()
	{
		BaseScene::FixedUpdate();
		//_testObject->FixedUpdate();
		//_mainCamera->FixedUpdate();
	}
	void TitleScene::Update()
	{
		
		_renderer->Update();
		BaseScene::Update();

		auto &kb = KeyboardInput::Instance();


		//_testObject->Update();
		//_mainCamera->Update();
	}
	void TitleScene::Draw()
	{
		_renderer->BeginFrame();
		BaseScene::Draw();	
		_renderer->Draw();
		_renderer->EndFrame();
	}
}
