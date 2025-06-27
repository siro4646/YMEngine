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
				m_lastFixedDeltaTime = fixedDeltaTime_.elapsedTime();
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

		bool IsWindowFocus();

		inline Window *GetWindow() const
		{
			return m_window.get();
		}
		inline float GetDeltaTime()
		{
			return ImGui::GetIO().DeltaTime;
			//return ym::config::kDeltaTime;
			return deltaTime_.elapsedTime();			
		}
		inline float GetFixedDeltaTime()
		{
			return m_lastFixedDeltaTime;
		}
		/// <summary>
		/// èâä˙âª
		/// </summary>
		bool Init();

		/// <summary>
		/// é¿çs
		/// </summary>
		void Run();

		/// <summary>
		/// å„èàóù
		/// </summary>
		void Terminate();
	private:
		float m_lastFixedDeltaTime = 0;
	};
}