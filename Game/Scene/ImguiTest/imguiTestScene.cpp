#include "imguiTestScene.h"
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
	void ImguiTestScene::Init()
	{
		BaseScene::Init();

		ym::ConsoleLog("ImguiTestScene::Init()\n");
		_renderer = Renderer::Instance();
		_mainCamera = std::make_shared<MainCamera>();
		gameObjectManager_->AddGameObject(_mainCamera);

		auto sceneOcTree = gameObjectManager_->AddGameObject(std::make_shared<SceneOcTree>());

		gameObjectManager_->AddGameObject(std::make_shared<SphereMap>());
	}
	void ImguiTestScene::UnInit()
	{
		BaseScene::UnInit();
		ym::ConsoleLog("ImguiTestScene::UnInit()\n");

		//_testObject->Uninit();
		//_mainCamera->Uninit();
	}
	void ImguiTestScene::FixedUpdate()
	{
		BaseScene::FixedUpdate();
		//_testObject->FixedUpdate();
		//_mainCamera->FixedUpdate();
	}
	void ImguiTestScene::Update()
	{
		static int c = 0;
		static Timer t;
		_renderer->Update();
		/*if (t.elapsedTime() > 2)
		{
			t.reset();
			if (c < 1000)
			{
				gameObjectManager_->AddGameObject(std::make_shared<PointLight>());
				c++;
			}
		}*/

		BaseScene::Update();
		//_testObject->Update();
		//_mainCamera->Update();
	}
	void ImguiTestScene::Draw()
	{
		_renderer->BeginFrame();
		BaseScene::Draw();
		ImGui::Begin("ImguiTestScene");
		ImGui::Text(u8"‚¨‚Í‚æ‚¤ Š¿Žš");
		ImGui::Text("Framerate(avg) %.3f ms/frame (%.If FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::Button("Add PointLight"))
		{

		}
		ImGui::End();

		_renderer->Draw();
		_renderer->EndFrame();
	}
}
