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

#include "utility/fileStream/fileObserver.h"

#include "gpuParticle/gpuParticle.h"

#include "directX12/shader/shaderLibrary.h"

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

		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);


		//pixの初期化
		//PIXLoadLatestWinPixGpuCapturerLibrary();

		srand((unsigned int)time(nullptr));
		
		ym::CpuTimer::Initialize();
		ym::GlobalRandom.SetSeed(rand()%10000000000);
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
		DragAcceptFiles(m_window->GetWndHandle(), TRUE);
		
		AllInitilizeManager();

		return true;

	}

	void Application::Run()
	{
		auto _renderer = Renderer::Instance();

		auto input = InputManager::Instance();
		auto keybord = KeyboardInput::Instance();
		auto sceneManager = ym::SceneManager::Instance();


		while (1)
		{
			deltaTime_.reset();

			if (!m_window->ProcessMessage())
			{
				break;
			}
			input.Poll();

			/*if (keybord.GetKey("RSHIFT"))
			{
				ym::ConsoleLogRelease("RSHIFT pressed, speed increased to 1.0f\n");
			}*/
			sceneManager->Update();
			// フレームレートの同期
				
			if (SyncFrameRate())
			{
				sceneManager->FixedUpdate();
				sceneManager->Draw();
			}
			DropFileManager::Instance().Update();
			input.Update();
			//ym::ConsoleLogRelease("%f秒\n",deltaTime_.elapsedTime());
		}

	}

	void Application::Terminate()
	{
		ym::ConsoleLog("Application::Terminate()\n");
		DragAcceptFiles(m_window->GetWndHandle(), FALSE);
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

		auto &shaderLibrary = ShaderLibrary::Instance();
		shaderLibrary.Init();

		auto lightManager = LightManager::Instance();
		lightManager->Init();

		auto renderTargetManager = RenderTargetManager::Instance();
		renderTargetManager->Init();

		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		sceneRenderRegistrar->Init();

		auto soundManager = SoundManager::GetInstance();
		soundManager->Init();

		ym::InputManager::Instance().Initialize(m_window->GetWndHandle());

		auto sceneManager = ym::SceneManager::Instance();

		//ym::ParticleSystem::Instance()->Init();

	}
	void Application::Delete()
	{

		//ym::ParticleSystem::Instance()->Uninit();

		auto sm = SceneManager::Instance();
		if (sm)
		{
			sm->Terminate();
		}

		ShaderLibrary::Instance().UnInit();

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

		auto fileObserver = FileObserver::Instance();
		if (fileObserver)
		{
			fileObserver->Uninit();
		}

		m_window.reset();

		if (m_instance != nullptr)
		{

			delete m_instance;
			m_instance = nullptr;
		}



	}
	bool Application::IsWindowFocus()
	{
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			return false;
		}
		return m_window->IsWindowFocus();
	}
}	// namespace ym