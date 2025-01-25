#include "gameObjectManager.h"
#include "gameObject.h"

#include <algorithm>
#include <mutex>
#include <execution> // ������s�|���V�[�𗘗p����ꍇ
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

		// ����� FixedUpdate �����s
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type == Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(deleteMutex); // �폜���X�g�ւ̃A�N�Z�X��ی�
				objectsToDelete.push_back(gameObject);
			}
			else {
				std::lock_guard<std::mutex> lock(fixedUpdateMutex);// FixedUpdate�̃A�N�Z�X��ی�
				gameObject->FixedUpdate();
			}
			});

		// �폜�����̓V���O���X���b�h�Ŏ��s
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
		//	//��������

		//}
		
	}

	void GameObjectManager::Update() {
		std::vector<std::shared_ptr<Object>> objectsToDelete;
		std::mutex deleteMutex;
		std::mutex updateMutex;

		// ����� Update �����s
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type == Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(deleteMutex); // �폜���X�g�ւ̃A�N�Z�X��ی�
				objectsToDelete.push_back(gameObject);
			}
			else {
				std::lock_guard<std::mutex> lock(updateMutex);// Update�̃A�N�Z�X��ی�
				gameObject->Update();
			}
			});

		// �폜�����̓V���O���X���b�h�Ŏ��s
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
		//	//��������

		//}
	}

	void GameObjectManager::Draw()
	{

		std::mutex drawMutex;


		// ����� Update �����s
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type != Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(drawMutex);// Update�̃A�N�Z�X��ی�
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
		
		ym::ConsoleLog("FindGameObjects�֐����Ă΂�܂���:%s",name);

		// ��v����I�u�W�F�N�g���i�[����z��
		std::vector<std::shared_ptr<Object>> objects;

		// gameObjects_���󂩊m�F
		if (gameObjects_.empty()) {
			ym::ConsoleLog("gameObjects_�͋�ł��B");
			return objects;
		}

		// �e�I�u�W�F�N�g���m�F
		for (const auto &gameObject : gameObjects_) {
			if (!gameObject) { // null�`�F�b�N
				ym::ConsoleLog("�x��: gameObjects_��null�̃I�u�W�F�N�g������܂��B");
				continue;
			}

			if (gameObject->name == name) { // ���O�̔�r
				objects.push_back(gameObject);
			}
		}

		ym::ConsoleLog("��v����I�u�W�F�N�g��:&s",std::to_string(objects.size()));
		return objects;
	}

}