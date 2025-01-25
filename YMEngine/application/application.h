#pragma once

#include "config/system.h"

namespace ym
{
	class Window;
	class Renderer;

	class Application
	{
	private:
		Application();
		~Application();
		static Application *m_instance;

		std::shared_ptr<Window> m_window;

		

		//Renderer *m_renderer = nullptr;

		Timer deltaTime_;
		Timer fixedDeltaTime_;


		inline bool SyncFrameRate()
		{
			if (fixedDeltaTime_.elapsedTime() > 1.0f / (ym::config::kFrameRate))
			{
				fixedDeltaTime_.reset();
				return true;
			}
			return false;

		}

		void AllInitilizeManager();

		void Delete();

	public:
		static Application *Instance()
		{
			if (m_instance == nullptr)
			{
				m_instance = new Application();
			}
			return m_instance;
		}

		inline Window *GetWindow() const
		{
			return m_window.get();
		}
		inline float GetDeltaTime()
		{
			return ym::config::kDeltaTime;
			return deltaTime_.elapsedTime();			
		}
		/// <summary>
		/// ������
		/// </summary>
		bool Init();

		/// <summary>
		/// ���s
		/// </summary>
		void Run();

		/// <summary>
		/// �㏈��
		/// </summary>
		void Terminate();

	};
}