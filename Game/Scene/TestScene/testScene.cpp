#include "testScene.h"
#include "renderer/renderer.h"

#include "Game/Object/TestObject.h"

#include "gameFrameWork/requiredObject/mainCamera.h"

namespace ym
{
	void TestScene::Init()
	{
		BaseScene::Init();

		ym::ConsoleLog("TestScene::Init()\n");
		_renderer = Renderer::Instance();

		_testObject = std::make_shared<TestObject>();
		gameObjectManager_->AddGameObject(_testObject);
		//_testObject->Init();
		_mainCamera = std::make_shared<MainCamera>();
		gameObjectManager_->AddGameObject(_mainCamera);
		//_mainCamera->Init();
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
		_renderer->Update();
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
