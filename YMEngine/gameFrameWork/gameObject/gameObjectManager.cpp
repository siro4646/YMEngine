#include "gameObjectManager.h"
#include "gameObject.h"

#include <algorithm>
#include <mutex>
#include <execution> // 並列実行ポリシーを利用する場合
#include <thread>

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


	void GameObjectManager::FixedUpdate() {

		std::vector<std::shared_ptr<Object>> objectsToDelete;
		std::mutex deleteMutex;
		std::mutex fixedUpdateMutex;

		// 並列に FixedUpdate を実行
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type == Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(deleteMutex); // 削除リストへのアクセスを保護
				objectsToDelete.push_back(gameObject);
			}
			else {
				std::lock_guard<std::mutex> lock(fixedUpdateMutex);// FixedUpdateのアクセスを保護
				gameObject->FixedUpdate();
			}
			});

		// 削除処理はシングルスレッドで実行
		for (auto &gameObject : objectsToDelete) {
			gameObject->Uninit();
			std::lock_guard<std::mutex> lock(deleteMutex);
			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
		}

		//std::vector<std::shared_ptr<Object>> objectsToDelete;
		//
		//for (auto obj : gameObjects_)
		//{
		//	if (obj->type == Object::Type::Delete)
		//	{
		//		objectsToDelete.push_back(obj);
		//	}
		//	else
		//	{
		//		obj->FixedUpdate();
		//	}
		//}

		//for (auto deleteObj : objectsToDelete)
		//{
		//	//消す処理

		//}
		
	}

	void GameObjectManager::Update() {
		std::vector<std::shared_ptr<Object>> objectsToDelete;
		std::mutex deleteMutex;
		std::mutex updateMutex;

		// 並列に Update を実行
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type == Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(deleteMutex); // 削除リストへのアクセスを保護
				objectsToDelete.push_back(gameObject);
			}
			else {
				std::lock_guard<std::mutex> lock(updateMutex);// Updateのアクセスを保護
				gameObject->Update();
			}
			});

		// 削除処理はシングルスレッドで実行
		for (auto &gameObject : objectsToDelete) {
			gameObject->Uninit();
			std::lock_guard<std::mutex> lock(deleteMutex);
			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
		}

		//std::vector<std::shared_ptr<Object>> objectsToDelete;

		//for (auto obj : gameObjects_)
		//{
		//	if (obj->type == Object::Type::Delete)
		//	{
		//		objectsToDelete.push_back(obj);
		//	}
		//	else
		//	{
		//		obj->Update();
		//	}
		//}

		//for (auto deleteObj : objectsToDelete)
		//{
		//	//消す処理

		//}
	}

	void GameObjectManager::Draw()
	{

		std::mutex drawMutex;


		// 並列に Update を実行
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type != Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(drawMutex);// Updateのアクセスを保護
				gameObject->Draw();				
			}
			});

		/*for (auto &gameObject : gameObjects_)
		{
			if (gameObject->type != Object::Type::Delete)
			gameObject->Draw();
		}*/
	}

	void GameObjectManager::Uninit()
	{
		gameObjects_.clear();
	}

	std::shared_ptr<Object> GameObjectManager::AddGameObject(std::shared_ptr<Object> gameObject, Object *const parent)
	{	

		gameObjects_.push_back(gameObject);
		gameObject->objectManager = this;
		gameObject->Init();
		return gameObject;
	}

	void GameObjectManager::RemoveGameObject(std::shared_ptr<Object> gameObject)
	{
	}

	std::vector<std::shared_ptr<Object>> GameObjectManager::FindGameObjects(const std::string &name)
	{
		
		ym::ConsoleLog("FindGameObjects関数が呼ばれました:%s",name);

		// 一致するオブジェクトを格納する配列
		std::vector<std::shared_ptr<Object>> objects;

		// gameObjects_が空か確認
		if (gameObjects_.empty()) {
			ym::ConsoleLog("gameObjects_は空です。");
			return objects;
		}

		// 各オブジェクトを確認
		for (const auto &gameObject : gameObjects_) {
			if (!gameObject) { // nullチェック
				ym::ConsoleLog("警告: gameObjects_にnullのオブジェクトがあります。");
				continue;
			}

			if (gameObject->name == name) { // 名前の比較
				objects.push_back(gameObject);
			}
		}

		ym::ConsoleLog("一致するオブジェクト数:&s",std::to_string(objects.size()));
		return objects;
	}

}