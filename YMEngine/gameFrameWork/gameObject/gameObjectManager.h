#pragma once

namespace ym
{
	class Object;

	class GameObjectManager
	{
	public:
		GameObjectManager();
		~GameObjectManager();

		void Init();
		void FixedUpdate();
		void Update();
		void Draw();
		void Uninit();

		std::shared_ptr<Object> AddGameObject(std::shared_ptr<Object> gameObject,Object *const parent = nullptr);
		void RemoveGameObject(std::shared_ptr<Object> gameObject);

		std::vector<std::shared_ptr<Object>> GetGameObjects() const { return gameObjects_; }

	private:
		std::vector<std::shared_ptr<Object>> gameObjects_;
	};

}