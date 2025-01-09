#pragma once
#include "gameFrameWork/gameObject/gameObjectManager.h"

namespace ym
{
	class GameObjectManager;
    class BaseScene {

    public:
        virtual ~BaseScene() {};
        virtual void Init()
        {
			gameObjectManager_ = std::make_shared<GameObjectManager>();
        }
        virtual void UnInit()
        {
			gameObjectManager_->Uninit();
        }
        virtual void FixedUpdate()
        {
			gameObjectManager_->FixedUpdate();
        }
        virtual void Update()
        {
			gameObjectManager_->Update();
        }
        virtual void Draw()
        {
			gameObjectManager_->Draw();
        }
    protected:
		std::shared_ptr<GameObjectManager> gameObjectManager_;
    };
} // namespace ym