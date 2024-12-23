#include "application.h"
#include "winAPI/window/window.h"
#include "Renderer/renderer.h"

#include "utility/inputSystem/inputManger/inputManger.h"

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
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);


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
		if (!_renderer->Init(m_window->GetWndHandle(), ym::config::kResolutionWidth, ym::config::kResolutionHeight,ym::ColorSpaceType::Rec2020))
		{
			return false;
		}
		ym::InputManager::GetInstance().Initialize(m_window->GetWndHandle());

		

		return true;

	}

	void Application::Run()
	{
		auto _renderer = Renderer::Instance();

		while (1)
		{
			if (!m_window->ProcessMessage())
			{
				break;
			}
			else
			{
				// フレームレートの同期
				if (SyncFrameRate())
				{

					_renderer->BeginFrame();
					_renderer->EndFrame();

					
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