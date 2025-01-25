#include "renderer.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"
#include "swapChain/swapChain.h"
#include "winAPI/window/window.h"

#include "camera/cameraManager.h"
#include "camera/camera.h"

#include "resource/resourceStateTracker.h"
#include "resource/resourceManager.h"

#include "texture/texture.h"
#include "textureView/textureView.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "rootSignature/rootSignature.h"
#include "descriptorSet/descriptorSet.h"
#include "pipelineState/pipelineState.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"


#include "material/postProcess/mosaic/mosaicMaterial.h"
#include "material/postProcess/grayScale/grayScaleMaterial.h"
#include "material/postProcess/blur/blurMaterial.h"
#include "material/postProcess/lightPass/lightPassMaterial.h"
#include "material/postProcess/lumPass/lumPassMaterial.h"


#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "imgui/imguiRender.h"

#include "gameFrameWork/light/lightManager.h"

#include "renderTargetManager/renderTargetManager.h"


namespace ym
{
	std::shared_ptr<Device> Renderer::device_{};
	std::shared_ptr<CommandList> Renderer::pGraphicCommandList_{};
	std::shared_ptr<CommandList> Renderer::pRenderTextureCommandList_{};
	std::shared_ptr<CommandList> Renderer::pComputeCommandList_{};
	std::shared_ptr<CommandList> Renderer::pCopyCommandList_{};

	bool Renderer::Init(Window*win, u32 width, u32 height, ColorSpaceType csType)
	{
		ym::ConsoleLog("Renderer::Init\n");
		device_ = std::make_shared<Device>();
		if (!device_)
		{
			return false;
		}

		if (!device_->Init(win->GetWndHandle(), width, height, csType))
		{
			return false;
		}
		pGraphicCommandList_ = std::make_shared<CommandList>();
		if (!pGraphicCommandList_)
		{
			return false;
		}
		if (!pGraphicCommandList_->Init(device_.get(),&device_->GetGraphicsQueue()))
		{
			return false;
		}	
		pComputeCommandList_ = std::make_shared<CommandList>();
		if (!pComputeCommandList_)
		{
			return false;
		}
		if (!pComputeCommandList_->Init(device_.get(), &device_->GetComputeQueue()))
		{
			return false;
		}
		pRenderTextureCommandList_ = std::make_shared<CommandList>();
		if (!pRenderTextureCommandList_)
		{
			return false;
		}
		if (!pRenderTextureCommandList_->Init(device_.get(), &device_->GetGraphicsQueue()))
		{
			return false;
		}
		pCopyCommandList_ = std::make_shared<CommandList>();
		if (!pCopyCommandList_)
		{
			return false;
		}
		if (!pCopyCommandList_->Init(device_.get(), &device_->GetCopyQueue()))
		{
			return false;
		}

		CreateRenderTargets(width, height);

		CreateDepthStencil(width, height);

		CreateDummyTextures();

		CreateSceneBuffers();

		CreateSceneShaders();

		CreateSceneRootSignature();

		CreateSceneDescriptorSet();

		CreateScenePipelineState();

		CreateSceneSampler();


		auto pPM = PostProcessManager::Instance();
		pPM->Init();

		mosaicMaterial_ = std::make_shared<MosaicMaterial>();
		mosaicMaterial_->Init();

		grayScaleMaterial_ = std::make_shared<GrayScaleMaterial>();
		grayScaleMaterial_->Init();

		blurMaterial_ = std::make_shared<BlurMaterial>();
		blurMaterial_->Init();

		lightPassMaterial_ = std::make_shared<LightPassMaterial>();
		lightPassMaterial_->Init();

		lumPassMaterial_ = std::make_shared<LumPassMaterial>();
		lumPassMaterial_->Init();

		pPM->AddPostProcessMaterial(lightPassMaterial_.get());
		pPM->AddPostProcessMaterial(lumPassMaterial_.get());

		//usePostProcess_[0] = true;


		//pPM->AddPostProcessMaterial(blurMaterial_.get());
		//pPM->AddPostProcessMaterial(mosaicMaterial_.get());
		//pPM->AddPostProcessMaterial(grayScaleMaterial_.get());
		window_ = win;
		//ビューポートの設定
		m_viewPort.Width = static_cast<float>(width);
		m_viewPort.Height = static_cast<float>(height);
		m_viewPort.MaxDepth = 1.0f;
		m_viewPort.MinDepth = 0.0f;
		m_viewPort.TopLeftX = 0;
		m_viewPort.TopLeftY = 0;
		//シザー矩形の設定
		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = width;
		m_scissorRect.bottom = height;

		auto &cameraManager = CameraManager::Instance();
		cameraManager.CreateCamera("mainCamera");
		
		return true;
	}
	void Renderer::Uninit()
	{
		if (isUninited)return;

		device_->WaitForGraphicsCommandQueue();
		device_->WaitForComputeCommandQueue();
		device_->WaitForCopyCommandQueue();

		//シーンバッファの開放
		sceneVertexBuffer_.reset();
		sceneVertexBufferView_.reset();
		sceneIndexBuffer_.reset();
		sceneIndexBufferView_.reset();

		//シーンパイプラインステートの開放
		scenePipelineState_.reset();

		//シーンルートシグネチャの開放
		sceneRootSignature_.reset();

		//シーンシェーダの開放
		sceneVS_.reset();
		scenePS_.reset();

		//シーンサンプラの開放
		sceneSampler_.reset();

		//シーンディスクリプタセットの開放
		sceneDescriptorSet_.reset();

		//レンダーターゲットの開放
		for (auto &tex : sceneRenderTextures)
		{
			tex.reset();
		}
		for (auto &texView : sceneRenderTargetTexViews)
		{
			texView.reset();
		}
		for (auto &rtv : sceneRenderTargetViews)
		{
			rtv.reset();
		}

		//デプスステンシルの開放
		depthStencilTexture_.reset();
		depthStencilView_.reset();
		depthStencilTexView_.reset();

		auto pPM = PostProcessManager::Instance();
		pPM->Uninit();

		mosaicMaterial_.reset();
		grayScaleMaterial_.reset();
		blurMaterial_.reset();
		lightPassMaterial_.reset();
		lumPassMaterial_.reset();



		for (auto &tex : dummyTextures_)
		{
			tex.reset();
		}
		for (auto &texView : dummyTextureViews_)
		{
			texView.reset();
		}

		auto &cameraManager = CameraManager::Instance();
		cameraManager.Uninit();

		if (window_)
		{
			window_ = nullptr;
		}

		pCopyCommandList_->Uninit();
		pComputeCommandList_->Uninit();
		pGraphicCommandList_->Uninit();
		device_->Uninit();



		//テクスチャマップの開放
		//ToDo:あとで開放用クラスをさくせい　
		Texture::ResourceRelease();

		auto rM = ResourceManager::Instance();
		rM->Uninit();

		auto rST = ResourceStateTracker::Instance();
		rST->Uninit();

		isUninited = true;

	}
	void Renderer::Update()
	{
		auto pPM = PostProcessManager::Instance();
		pPM->Update();
		auto lightManager = LightManager::Instance();
		lightManager->Update();
		auto &input = KeyboardInput::GetInstance();
		//if (input.GetKeyDown("1"))
		//{
		//	auto pPM = PostProcessManager::Instance();
		//	pPM->AddPostProcessMaterial(mosaicMaterial_.get());
		//}
		//if (input.GetKeyDown("2"))
		//{
		//	auto pPM = PostProcessManager::Instance();
		//	pPM->AddPostProcessMaterial(grayScaleMaterial_.get());
		//}
		//if (input.GetKeyDown("3"))
		//{
		//	auto pPM = PostProcessManager::Instance();
		//	pPM->AddPostProcessMaterial(blurMaterial_.get());
		//}
		//if (input.GetKeyDown("4"))
		//{
		//	auto pPM = PostProcessManager::Instance();
		//	pPM->RemovePostProcessMaterial(mosaicMaterial_.get());
		//}
		//if (input.GetKeyDown("5"))
		//{
		//	auto pPM = PostProcessManager::Instance();
		//	pPM->RemovePostProcessMaterial(grayScaleMaterial_.get());
		//}
		//if (input.GetKeyDown("6"))
		//{
		//	auto pPM = PostProcessManager::Instance();
		//	pPM->RemovePostProcessMaterial(blurMaterial_.get());
		//}

		/*for (int i = 0; i < (int)PostProcessType::Max; ++i)
		{
			if (usePostProcess_[i])
			{
				if (i == (int)PostProcessType::Mosaic)
				{
					auto pPM = PostProcessManager::Instance();
					pPM->AddPostProcessMaterial(mosaicMaterial_.get());
				}
				if (i == (int)PostProcessType::GrayScale)
				{
					auto pPM = PostProcessManager::Instance();
					pPM->AddPostProcessMaterial(grayScaleMaterial_.get());
				}
				if (i == (int)PostProcessType::Blur)
				{
					auto pPM = PostProcessManager::Instance();
					pPM->AddPostProcessMaterial(blurMaterial_.get());
				}
			}
			else
			{
				if (i == (int)PostProcessType::Mosaic)
				{
					auto pPM = PostProcessManager::Instance();
					pPM->RemovePostProcessMaterial(mosaicMaterial_.get());
				}
				if (i == (int)PostProcessType::GrayScale)
				{
					auto pPM = PostProcessManager::Instance();
					pPM->RemovePostProcessMaterial(grayScaleMaterial_.get());
				}
				if (i == (int)PostProcessType::Blur)
				{
					auto pPM = PostProcessManager::Instance();
					pPM->RemovePostProcessMaterial(blurMaterial_.get());
				}
				
			}
		}*/

		//if (input.GetKeyDown("8"))
		//{
		//	blurValue_ -= 0.1f;
		//}




	}
	void Renderer::Resize(u32 width, u32 height)
	{
		device_->WaitForGraphicsCommandQueue();
		device_->Resize(width, height);
		m_viewPort.Width = static_cast<float>(width);
		m_viewPort.Height = static_cast<float>(height);
		m_scissorRect.right = width;
		m_scissorRect.bottom = height;
	}
	void Renderer::BeginFrame()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto nextbbidx = (bbidx + 1) % ym::SwapChain::kFrameCount;
		auto cmd = pGraphicCommandList_;

		


		auto sceneTex = GetSceneRenderTexture(bbidx, MultiRenderTargets::Color);
		auto sceneTex1 = GetSceneRenderTexture(bbidx, MultiRenderTargets::Normal);
		auto sceneTex2 = GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance);
		auto sceneTex3 = GetSceneRenderTexture(bbidx, MultiRenderTargets::WorldPos);

		auto sceneRtv = GetSceneRenderTargetView(bbidx, MultiRenderTargets::Color);
		auto sceneRtv1 = GetSceneRenderTargetView(bbidx, MultiRenderTargets::Normal);
		auto sceneRtv2 = GetSceneRenderTargetView(bbidx, MultiRenderTargets::HighLuminance);
		auto sceneRtv3 = GetSceneRenderTargetView(bbidx, MultiRenderTargets::WorldPos);

		D3D12_CPU_DESCRIPTOR_HANDLE  sceneRtvs[] = {
			sceneRtv->GetDescInfo().cpuHandle,
			sceneRtv1->GetDescInfo().cpuHandle,
			sceneRtv2->GetDescInfo().cpuHandle,
			sceneRtv3->GetDescInfo().cpuHandle
		};



		cmd->TransitionBarrier(sceneTex, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmd->TransitionBarrier(sceneTex1, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmd->TransitionBarrier(sceneTex2, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmd->TransitionBarrier(sceneTex3, D3D12_RESOURCE_STATE_RENDER_TARGET);


			//sceneRenderTargetViews[bbidx].get();
		auto sceneDsvTex = depthStencilTexture_.get();
		auto sceneDsv = depthStencilView_.get();
		cmd->TransitionBarrier(sceneDsvTex, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		float clearColor[] = { 0.5,1,1,1 };

		auto rtManager = RenderTargetManager::Instance();
		rtManager->BeginRenderTarget({ *sceneRtv,*sceneRtv1,*sceneRtv2,*sceneRtv3 }, { *sceneTex,*sceneTex1,*sceneTex2,*sceneTex3 }, sceneDsv, sceneDsvTex, true);

		SetViewPort();

		//ImGuiのフレーム開始
		auto pImGui = ImGuiRender::Instance();
		pImGui->NewFrame();
	}
	void Renderer::Draw()
	{
		/*ImGui::Begin("PostProcess");
		for (int i = 0; i < (int)PostProcessType::Max; ++i)
		{
			ImGui::PushID(i);
			ImGui::Checkbox("aaaa", &usePostProcess_[i]);
			ImGui::PopID();
		}
		ImGui::End();*/

	}
	void Renderer::EndFrame()
	{



		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmd = pGraphicCommandList_;
		auto pCmdList = cmd->GetCommandList();

		
		//===========================================================

		/*auto rtv = device_->GetSwapChain().GetDescHandle(bbidx);
		auto scTex = device_->GetSwapChain().GetCurrentTexture();
		auto dsvTex = device_->GetSwapChain().GetDepthStencilTexture();		

		cmd->TransitionBarrier(scTex, D3D12_RESOURCE_STATE_PRESENT);
		cmd->TransitionBarrier(dsvTex,  D3D12_RESOURCE_STATE_GENERIC_READ);*/

		//===========================================================



		auto sceneTex = GetSceneRenderTexture(bbidx, MultiRenderTargets::Color);
		auto sceneTex1 = GetSceneRenderTexture(bbidx, MultiRenderTargets::Normal);
		auto sceneTex2 = GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance);
		auto sceneTex3 = GetSceneRenderTexture(bbidx, MultiRenderTargets::WorldPos);
		auto sceneRtv = GetSceneRenderTargetView(bbidx, MultiRenderTargets::Color);
		auto sceneDsvTex = depthStencilTexture_.get();

		//シーンテクスチャをシェーダーに描画するために遷移
		cmd->TransitionBarrier(sceneTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmd->TransitionBarrier(sceneTex1, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmd->TransitionBarrier(sceneTex2, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmd->TransitionBarrier(sceneTex3, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmd->TransitionBarrier(sceneDsvTex, D3D12_RESOURCE_STATE_GENERIC_READ);

		auto ds = sceneDescriptorSet_.get();
		auto pPM = PostProcessManager::Instance();

		pPM->Draw();

		auto resultTex = pPM->GetResultTexture(bbidx);
		auto resultTexView = pPM->GetResultTextureView(bbidx);

		//バックバッファをクリア
		auto swapChainTex = device_->GetSwapChain().GetCurrentTexture();
		auto rtv = device_->GetSwapChain().GetDescHandle(bbidx);

		cmd->TransitionBarrier(resultTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmd->TransitionBarrier(swapChainTex,D3D12_RESOURCE_STATE_RENDER_TARGET);
		auto clearColor = swapChainTex->GetTextureDesc().clearColor ;

		auto rtManager = RenderTargetManager::Instance();
		rtManager->BeginRenderTarget({ *device_->GetSwapChain().GetRenderTargetView(bbidx) }, { *swapChainTex }, nullptr, nullptr, true);
	
		//ポストプロセスのリザルトをバックバッファに描画


		//SetViewPort();

		cmd->GetCommandList()->SetPipelineState(scenePipelineState_->GetPSO());
		ds->Reset();
		ds->SetPsSrv(0, resultTexView->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sceneSampler_->GetDescInfo().cpuHandle);

		cmd->SetGraphicsRootSignatureAndDescriptorSet(sceneRootSignature_.get(), ds);

		cmd->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmd->GetCommandList()->IASetVertexBuffers(0, 1, &sceneVertexBufferView_->GetView());
		cmd->GetCommandList()->IASetIndexBuffer(&sceneIndexBufferView_->GetView());
		cmd->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

		auto imgui = ImGuiRender::Instance();
		imgui->EndFrame();
		imgui->Render(cmd.get());
		//バックバッファをプレゼント

		cmd->TransitionBarrier(swapChainTex, D3D12_RESOURCE_STATE_PRESENT);
		//cmd->TransitionBarrier(sceneTex, D3D12_RESOURCE_STATE_GENERIC_READ);

		pGraphicCommandList_->Close();
		pRenderTextureCommandList_->Close();
		pComputeCommandList_->Close();
		pCopyCommandList_->Close();
		pGraphicCommandList_->Execute();
		pRenderTextureCommandList_->Execute();
		pComputeCommandList_->Execute();
		pCopyCommandList_->Execute();
		device_->WaitForGraphicsCommandQueue();
		device_->WaitForComputeCommandQueue();
		device_->WaitForCopyCommandQueue();
		device_->Present();
		cmd->Reset();
		pRenderTextureCommandList_->Reset();
		pComputeCommandList_->Reset();
		pCopyCommandList_->Reset();
	}
	void Renderer::CreateDummyTextures()
	{
		auto rM = ResourceManager::Instance();
		dummyTextures_.clear();
		dummyTextureViews_.clear();
		dummyTextures_.resize(DummyTex::Max);
		dummyTextureViews_.resize(DummyTex::Max);

		// Black
		{
			TextureDesc desc{};
			desc.initialState = D3D12_RESOURCE_STATE_COPY_DEST;
			desc.width = desc.height = 4;
			desc.depth = 1;
			desc.dimension = TextureDimension::Texture2D;
			desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.mipLevels = 1;

			std::vector<ym::u32> bin;
			bin.resize(64 * 4);
			for (auto &&pix : bin)
			{
				pix = 0x00000000;
			}

			dummyTextures_[DummyTex::Black] = std::make_unique<Texture>();
			if (!dummyTextures_[DummyTex::Black]->InitFromBin(device_.get(), pGraphicCommandList_.get(), desc, bin.data()))
			{
				return;
			}

			dummyTextureViews_[DummyTex::Black] = std::make_unique<TextureView>();
			if (!dummyTextureViews_[DummyTex::Black]->Init(device_.get(), dummyTextures_[DummyTex::Black].get()))
			{
				return;
			}
			rM->SetTexture("black", *dummyTextures_[DummyTex::Black].get());
			
		}

		// White
		{
			TextureDesc desc{};
			desc.initialState = D3D12_RESOURCE_STATE_COPY_DEST;
			desc.width = desc.height = 4;
			desc.depth = 1;
			desc.dimension = TextureDimension::Texture2D;
			desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.mipLevels = 1;

			std::vector<ym::u32> bin;
			bin.resize(64 * 4);
			for (auto &&pix : bin)
			{
				pix = 0xFFFFFFFF;
			}

			dummyTextures_[DummyTex::White] = std::make_unique<Texture>();
			if (!dummyTextures_[DummyTex::White]->InitFromBin(device_.get(), pGraphicCommandList_.get(), desc, bin.data()))
			{
				return;
			}

			dummyTextureViews_[DummyTex::White] = std::make_unique<TextureView>();
			if (!dummyTextureViews_[DummyTex::White]->Init(device_.get(), dummyTextures_[DummyTex::White].get()))
			{
				return;
			}
			rM->SetTexture("white", *dummyTextures_[DummyTex::White].get());
		}

		//Purple
		{
			TextureDesc desc{};
			desc.initialState = D3D12_RESOURCE_STATE_COPY_DEST;
			desc.width = desc.height = 4;
			desc.depth = 1;
			desc.dimension = TextureDimension::Texture2D;
			desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.mipLevels = 1;

			std::vector<ym::u32> bin;
			bin.resize(64 * 4);
			for (auto &&pix : bin)
			{
				pix = 0xFF880088;
			}

			dummyTextures_[DummyTex::Purple] = std::make_unique<Texture>();
			if (!dummyTextures_[DummyTex::Purple]->InitFromBin(device_.get(), pGraphicCommandList_.get(), desc, bin.data()))
			{
				return;
			}
			rM->SetTexture("purple", *dummyTextures_[DummyTex::Purple].get());
		}

		// FlatNormal
		{
			TextureDesc desc{};
			desc.initialState = D3D12_RESOURCE_STATE_COPY_DEST;
			desc.width = desc.height = 4;
			desc.depth = 1;
			desc.dimension = TextureDimension::Texture2D;
			desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.mipLevels = 1;

			std::vector<ym::u32> bin;
			bin.resize(64 * 4);
			for (auto &&pix : bin)
			{
				pix = 0xFF7F7FFF;
			}

			dummyTextures_[DummyTex::FlatNormal] = std::make_unique<Texture>();
			if (!dummyTextures_[DummyTex::FlatNormal]->InitFromBin(device_.get(), pGraphicCommandList_.get(), desc, bin.data()))
			{
				return;
			}

			dummyTextureViews_[DummyTex::FlatNormal] = std::make_unique<TextureView>();
			if (!dummyTextureViews_[DummyTex::FlatNormal]->Init(device_.get(), dummyTextures_[DummyTex::FlatNormal].get()))
			{
				return;
			}
			rM->SetTexture("flatNormal", *dummyTextures_[DummyTex::FlatNormal].get());
		}

		for (auto &&tex : dummyTextures_)
		{
			pGraphicCommandList_->TransitionBarrier(tex.get(), D3D12_RESOURCE_STATE_GENERIC_READ);
		}
	}
	void Renderer::CreateRenderTargets(u32 width, u32 height)
	{
		int frameCount = ym::SwapChain::kFrameCount*MultiRenderTargets::Max;

		sceneRenderTextures.resize(frameCount);
		sceneRenderTargetTexViews.resize(frameCount);
		sceneRenderTargetViews.resize(frameCount);

		ym::TextureDesc texDesc;
		texDesc.dimension = TextureDimension::Texture2D;
		texDesc.width = width;
		texDesc.height = height;
		texDesc.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texDesc.clearColor[0] = 0.5f;
		texDesc.clearColor[1] = 1.f;
		texDesc.clearColor[2] = 1.f;
		texDesc.clearColor[3] = 1.0f;
		texDesc.isRenderTarget = true;
		for (int i = 0; i < frameCount; ++i)
		{
			sceneRenderTextures[i] = std::make_shared<Texture>();
			//作成している物がColorの場合はtexDescのクリアカラーを水色にする
			if (i % MultiRenderTargets::Max == MultiRenderTargets::Color)
			{
				texDesc.clearColor[0] = 0.5f;
				texDesc.clearColor[1] = 1.f;
				texDesc.clearColor[2] = 1.f;
				texDesc.clearColor[3] = 1.0f;
			}
			else
			{
				texDesc.clearColor[0] = 0.0f;
				texDesc.clearColor[1] = 0.0f;
				texDesc.clearColor[2] = 0.0f;
				texDesc.clearColor[3] = 1.0f;
			}

			if (!sceneRenderTextures[i]->Init(device_.get(), texDesc))
			{
				return;
			}
			sceneRenderTargetViews[i] = std::make_shared<RenderTargetView>();
			if (!sceneRenderTargetViews[i]->Init(device_.get(), sceneRenderTextures[i].get()))
			{
				return;
			}
			sceneRenderTargetTexViews[i] = std::make_shared<TextureView>();
			if (!sceneRenderTargetTexViews[i]->Init(device_.get(), sceneRenderTextures[i].get()))
			{
				return;
			}	
		}

	}
	void Renderer::CreateDepthStencil(u32 width, u32 height)
	{
		ym::TextureDesc texDesc;
		texDesc.dimension = TextureDimension::Texture2D;
		texDesc.width = width;
		texDesc.height = height;
		texDesc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.isDepthBuffer = true;
		texDesc.clearDepth = 1.0f;
		texDesc.clearStencil = 0;

		depthStencilTexture_ = std::make_shared<Texture>();
		if (!depthStencilTexture_->Init(device_.get(), texDesc))
		{
			return;
		}
		depthStencilView_ = std::make_shared<DepthStencilView>();
		if (!depthStencilView_->Init(device_.get(), depthStencilTexture_.get()))
		{
			return;
		}
		depthStencilTexView_ = std::make_shared<TextureView>();
		if (!depthStencilTexView_->Init(device_.get(), depthStencilTexture_.get()))
		{
			return;
		}
	}
	void Renderer::CreateSceneBuffers()
	{		
		sceneVertexBuffer_ = std::make_shared<Buffer>();
		sceneVertexBufferView_ = std::make_shared<VertexBufferView>();
		sceneVertexBuffer_->Init(device_.get(), sizeof(sceneVertices_), sizeof(Vertex2D),BufferUsage::VertexBuffer,false,false);
		sceneVertexBufferView_->Init(device_.get(), sceneVertexBuffer_.get());
		sceneVertexBuffer_->UpdateBuffer(device_.get(), pGraphicCommandList_.get(), sceneVertices_, sizeof(sceneVertices_));

		sceneIndexBuffer_ = std::make_shared<Buffer>();
		sceneIndexBufferView_ = std::make_shared<IndexBufferView>();
		sceneIndexBuffer_->Init(device_.get(), sizeof(sceneIndices_), sizeof(u32), BufferUsage::IndexBuffer, false, false);
		sceneIndexBufferView_->Init(device_.get(), sceneIndexBuffer_.get());
		sceneIndexBuffer_->UpdateBuffer(device_.get(), pGraphicCommandList_.get(), sceneIndices_.data(), sizeof(sceneIndices_));

	}
	void Renderer::CreateSceneRootSignature()
	{
		sceneRootSignature_ = std::make_shared<RootSignature>();
		sceneRootSignature_->Init(device_.get(),sceneVS_.get(),scenePS_.get(),nullptr,nullptr,nullptr);
	}
	void Renderer::CreateSceneDescriptorSet()
	{
		sceneDescriptorSet_ = std::make_shared<DescriptorSet>();
	}
	void Renderer::CreateScenePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		};
		ym::GraphicsPipelineStateDesc desc;
		desc.rasterizer.fillMode = D3D12_FILL_MODE_SOLID;
		desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
		desc.rasterizer.isFrontCCW = false;
		desc.rasterizer.isDepthClipEnable = false;
		desc.multisampleCount = 1;

		desc.blend.sampleMask = UINT_MAX;
		desc.blend.rtDesc[0].isBlendEnable = false;
		desc.blend.rtDesc[0].srcBlendColor = D3D12_BLEND_ONE;
		desc.blend.rtDesc[0].dstBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpColor = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].srcBlendAlpha = D3D12_BLEND_ONE;
		desc.blend.rtDesc[0].dstBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].writeMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.depthStencil.isDepthEnable = false;
		desc.depthStencil.isDepthWriteEnable = false;

		desc.pRootSignature = sceneRootSignature_.get();
		desc.pVS = sceneVS_.get();
		desc.pPS = scenePS_.get();
		desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		desc.inputLayout.numElements = _countof(elementDescs);
		desc.inputLayout.pElements = elementDescs;
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = device_->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();

		scenePipelineState_ = std::make_shared<GraphicsPipelineState>();
		scenePipelineState_->Init(device_.get(), desc);
	}
	void Renderer::CreateSceneShaders()
	{
		sceneVS_ = std::make_shared<Shader>();
		scenePS_ = std::make_shared<Shader>();

		sceneVS_->Init(device_.get(), ShaderType::Vertex, "fullScreen");
		scenePS_->Init(device_.get(), ShaderType::Pixel, "fullScreen");
	}
	void Renderer::CreateSceneSampler()
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sceneSampler_ = std::make_shared<Sampler>();
		sceneSampler_->Initialize(device_.get(), desc);
	}
	void Renderer::SetViewPort()
	{
		auto cmd = pGraphicCommandList_;
		cmd->GetCommandList()->RSSetViewports(1, &m_viewPort);
		cmd->GetCommandList()->RSSetScissorRects(1, &m_scissorRect);
	}
}
