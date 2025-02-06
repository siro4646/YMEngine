#pragma once

namespace ym
{
	class Object;

	class SceneRenderRegistrar
	{
	public:
		static SceneRenderRegistrar *Instance()
		{
			static SceneRenderRegistrar instance;
			return &instance;
		}

		void Init();
		void Uninit();
		void Draw();
		void AddRenderObject(Object *object) { 
			//���ɓo�^����Ă��邩�ǂ������m�F
			for (auto obj : renderObjects_)
			{
				if (obj == object)
				{
					return;
				}
			}

			renderObjects_.push_back(object); 
		}
		void RemoveRenderObject(Object *object) { renderObjects_.erase(std::remove(renderObjects_.begin(), renderObjects_.end(), object), renderObjects_.end()); }
	private:
		std::vector<Object*> renderObjects_;
	};
}