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

		std::vector<std::shared_ptr<Object>> FindGameObjects(const std::string &name);

		std::vector<std::shared_ptr<Object>> GetGameObjects() const { return gameObjects_; }

		template <typename T>std::vector<std::shared_ptr<T>> FindGameObjects()
		{
			std::vector<std::shared_ptr<T>> result;
			for (auto obj : gameObjects_)
			{
				
				if (typeid(*obj) == typeid(T))
				{
					result.push_back(std::dynamic_pointer_cast<T>(obj));
				}
			}
			return result;
		}

		std::vector<std::shared_ptr<Object>> gameObjects_;
	private:
	};

}