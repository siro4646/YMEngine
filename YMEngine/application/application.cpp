#include "application.h"
#include "winAPI/window/window.h"
#include "Renderer/renderer.h"
#include "scene/sceneManager.h"

#include "texture/texture.h"


namespace ym
{
	Application *Application::m_instance = nullptr;


	LARGE_INTEGER ym::CpuTimer::frequency_;
	LARGE_INTEGER Timer::frequency;
	bool Timer::flag = false;

	Application::Application()
	{
	}

	Application::~Application()
	{
		Texture::ResourceRelease();
		//Terminate();
//#ifdef _DEBUG
//		ComPtr<IDXGIDebug1>dxgiDebug;
//		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
//			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
//			//dxgiDebug->Release();
//		}
//#endif
		
	}

	bool Application::Init()
	{
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);


		ym::CpuTimer::Initialize();
		deltaTime_.reset();

		m_window = std::make_shared<Window>();
		//newに失敗した場合
		if (!m_window)
		{
			return false;
		}

		if (!m_window->Create(ym::config::kResolutionWidth, ym::config::kResolutionHeight, "初期化中", " "))
		{
			return false;
		}
		auto _renderer = Renderer::Instance();

		if (!_renderer)
		{
			return false;
		}
		if (!_renderer->Init(m_window.get(), ym::config::kResolutionWidth, ym::config::kResolutionHeight, ym::ColorSpaceType::Rec2020))
		{
			return false;
		}
		ym::InputManager::GetInstance().Initialize(m_window->GetWndHandle());

		auto sceneManager = ym::SceneManager::Instance();
		
		

		return true;

	}

	void Application::Run()
	{
		auto _renderer = Renderer::Instance();

		auto input = InputManager::GetInstance();
		auto keybord = KeyboardInput::GetInstance();
		auto sceneManager = ym::SceneManager::Instance();

		while (1)
		{
			input.Update();
			if (!m_window->ProcessMessage())
			{
				break;
			}
			else
			{
				// フレームレートの同期
				if (SyncFrameRate())
				{
					if (keybord.GetKeyDown("Z"))
					{
						ym::ConsoleLog("Z\n");
						_renderer->Resize(ym::config::kResolutionWidth, ym::config::kResolutionHeight);
						ym::ConsoleLog("%d %d \n", ym::config::kResolutionWidth, ym::config::kResolutionHeight);
					}
					else if (keybord.GetKeyDown("X"))
					{
						ym::ConsoleLog("X\n");
						u32 x = 192;
						u32 y = 108;
						_renderer->Resize(x, y);
						ym::ConsoleLog("%d %d \n",x, y);
					}
					else if (keybord.GetKeyDown("C"))
					{
						ym::ConsoleLog("C\n");
						u32 x = 800;
						u32 y = 600;
						_renderer->Resize(x, y);
						ym::ConsoleLog("%d %d \n", x, y);
					}
					sceneManager->Update();
					sceneManager->Draw();
				}
			}
		}

	}

	void Application::Terminate()
	{
		CoUninitialize();
		Delete();
	}
	void Application::Delete()
	{

		if (m_instance != nullptr)
		{

			delete m_instance;
			m_instance = nullptr;
		}
	}
}	// namespace ym