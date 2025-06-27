#include "titleSprite.h"

#include "gameFrameWork/components/sprite/sprite.h"

#include "gameFrameWork/requiredObject/mainCamera.h"

#include "gameFrameWork/gameObject/gameObjectManager.h"

#include "Game/Object/board/gameBoard.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "gameFrameWork/sound/soundManager.h"

#include "scene/sceneManager.h"

#include "Game/Scene/TestScene/testScene.h"

namespace ym {

	void TitleSprite::Init()
	{	
		auto sprite = AddComponent<Sprite>().get();
		sprite->LoadTexture("Asset/Texture/title.png");

		camera = objectManager->FindGameObjects<MainCamera>()[0];

		camera->localTransform.Position = { 0,0,-2 };


		AddChild(camera);
		isGameStart = false;

	}

	void TitleSprite::FixedUpdate()
	{
		Object::FixedUpdate();
		if (isGameStart) {
			camera->localTransform.Position.z += 0.01;
			if (camera->localTransform.Position.z > 0)
			{

				camera->localTransform.Position.z = 0;
				type = Type::Delete;
			}
		}
	}

	void TitleSprite::Update()
	{
		Object::Update();
		auto &kb = KeyboardInput::Instance();
		if (kb.GetKeyDown("SPACE")&& !isGameStart)
		{
			isGameStart = true;
			//スタート音再生
			auto soundManager = SoundManager::GetInstance();

			soundManager->LoadMP3("startSE", "asset/sound/start.mp3");

			soundManager->LoadMP3("gameBGM", "asset/sound/game.mp3");



			soundManager->PlaySE2D("startSE");


		}
	}

	void TitleSprite::Draw()
	{
		Object::Draw();
	}

	void TitleSprite::Uninit()
	{
		Object::Uninit();
		SceneManager::Instance()->ChangeScene(std::make_unique<TestScene>());

		auto soundManager = SoundManager::GetInstance();

		soundManager->PlayBGM2D("gameBGM");
	}

	std::shared_ptr<Object> TitleSprite::Clone()
	{
		return std::make_shared<TitleSprite>(*this);
	}



}