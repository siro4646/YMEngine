#include "gameObjectManager.h"
#include "gameObject.h"

namespace ym
{

	GameObjectManager::GameObjectManager()
	{

	}

	GameObjectManager::~GameObjectManager()
	{

	}

	void GameObjectManager::Init()
	{
	}

	void GameObjectManager::FixedUpdate()
	{

		std::vector<std::shared_ptr<Object>> objectsToDelete;
		auto it = gameObjects_.begin();
		while (it != gameObjects_.end())
		{
			auto &gameObject = *it;
			if (gameObject->type == Object::Type::Delete)
			{
				objectsToDelete.push_back(gameObject);
			}
			else
			{
				gameObject->FixedUpdate();
			}
			++it;
		}

		for (auto &gameObject : objectsToDelete)
		{
			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
		}
	}

	void GameObjectManager::Update()
	{

		std::vector<std::shared_ptr<Object>> objectsToDelete;
		auto it = gameObjects_.begin();
		while (it != gameObjects_.end())
		{
			auto &gameObject = *it;
			if (gameObject->type == Object::Type::Delete)
			{
				objectsToDelete.push_back(gameObject);
			}
			else
			{
				gameObject->Update();
			}
			++it;
		}

		for (auto &gameObject : objectsToDelete)
		{
			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
		}
	}

	void GameObjectManager::Draw()
	{
		for (auto &gameObject : gameObjects_)
		{
			if (gameObject->type != Object::Type::Delete)
			gameObject->Draw();
		}
	}

	void GameObjectManager::Uninit()
	{
		gameObjects_.clear();
	}

	std::shared_ptr<Object> GameObjectManager::AddGameObject(std::shared_ptr<Object> gameObject, Object *const parent)
	{
		gameObject->Init();
		gameObjects_.push_back(gameObject);
		gameObject->objectManager = this;
		return gameObject;
	}

	void GameObjectManager::RemoveGameObject(std::shared_ptr<Object> gameObject)
	{
	}

}