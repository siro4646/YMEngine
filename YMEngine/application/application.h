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

		inline bool SyncFrameRate()
		{
			if (deltaTime_.elapsedMiliTime() > 1.0f / (ym::config::kFrameRate))
			{
				//ym::ConsoleLog("FPS: %f\n", 1.0f / deltaTime_.elapsedMiliTime());
				deltaTime_.reset();
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

	};
}