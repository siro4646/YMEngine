#include "application.h"
#include "winAPI/window/window.h"
#include "Renderer/renderer.h"
#include "scene/sceneManager.h"

#include "texture/texture.h"

#include "imgui/imguiRender.h"

#include "gameFrameWork/light/lightManager.h"

#include "renderTargetManager/renderTargetManager.h"

#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"

#include "gameFrameWork/sound/soundManager.h"

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

		


		//pixの初期化
		//PIXLoadLatestWinPixGpuCapturerLibrary();

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
		
		AllInitilizeManager();

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
			deltaTime_.reset();
			input.Update();
			if (!m_window->ProcessMessage())
			{
				break;
			}
			else
			{
				sceneManager->Update();
				// フレームレートの同期
				if (SyncFrameRate())
				{
					sceneManager->FixedUpdate();
					sceneManager->Draw();
				}
			}
			//ym::ConsoleLogRelease("%f秒\n",deltaTime_.elapsedTime());
		}

	}

	void Application::Terminate()
	{
		ym::ConsoleLog("Application::Terminate()\n");
		Delete();
		CoUninitialize();
	}
	void Application::AllInitilizeManager()
	{
		auto _renderer = Renderer::Instance();
		if (!_renderer)
		{
			return;
		}
		if (!_renderer->Init(m_window.get(), ym::config::kResolutionWidth, ym::config::kResolutionHeight, ym::ColorSpaceType::Rec2020))
		{
			return;
		}
		auto imgui = ImGuiRender::Instance();
		if (imgui)
		{
			imgui->Init();
		}
		auto lightManager = LightManager::Instance();
		lightManager->Init();

		auto renderTargetManager = RenderTargetManager::Instance();
		renderTargetManager->Init();

		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		sceneRenderRegistrar->Init();

		auto soundManager = SoundManager::GetInstance();
		soundManager->Init();

		ym::InputManager::GetInstance().Initialize(m_window->GetWndHandle());

		auto sceneManager = ym::SceneManager::Instance();
	}
	void Application::Delete()
	{

		auto sm = SceneManager::Instance();
		if (sm)
		{
			sm->Terminate();
		}
		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		if (sceneRenderRegistrar)
		{
			sceneRenderRegistrar->Uninit();
		}

		auto soundManager = SoundManager::GetInstance();
		if (soundManager)
		{
			soundManager->Uninit();
		}

		auto renderTargetManager = RenderTargetManager::Instance();
		if (renderTargetManager)
		{
			renderTargetManager->Uninit();
		}

		auto lightManager = LightManager::Instance();
		if (lightManager)
		{
			lightManager->Uninit();
		}

		auto imgui = ImGuiRender::Instance();
		if (imgui)
		{
			imgui->Uninit();
		}
		auto _renderer = Renderer::Instance();
		if (_renderer)
		{
			_renderer->Uninit();
		}

		m_window.reset();

		if (m_instance != nullptr)
		{

			delete m_instance;
			m_instance = nullptr;
		}


	}
}	// namespace ym