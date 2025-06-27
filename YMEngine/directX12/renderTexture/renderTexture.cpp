#include "renderTexture.h"

#include "renderTargetManager/renderTargetManager.h"
#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"

#include "swapChain/swapChain.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"



namespace ym
{
	void RenderTexture::Init()
	{
		depthStencilTexture_ = std::make_shared<ym::Texture>();
		depthStencilView_ = std::make_shared<ym::DepthStencilView>();

		//エラー回避のためにメインカメラをセット
		useCameraName_ = "mainCamera";

		bbidx = 0;
	}

	void RenderTexture::FixedUpdate()
	{
	}

	void RenderTexture::Update()
	{
	}

	bool RenderTexture::Draw(Object *owner)
	{
		static Object *now_ = nullptr;
		//まだ呼ばれていない
		if (!now_) now_ = owner;
		//呼ばれているなら何もしない
		else if (now_)return false;

		if (!isCreated_)return false;

		auto renderTargetManager = ym::RenderTargetManager::Instance();
		auto sceneRenderRegistrar = ym::SceneRenderRegistrar::Instance();
		auto &cManager = CameraManager::Instance();
		auto prvCamera = cManager.GetMainCamera();
		//bbidx = Renderer::Instance()->GetDevice()->GetSwapChain().GetFrameIndex();

		auto cmdList = Renderer::Instance()->GetGraphicCommandList();

		cManager.SetMainCamera(useCameraName_);
		vector<RenderTargetView>rtvs;
		vector<Texture>rts;
		for (int i = 0; i < renderTargetTextureView_.size(); i++)
		{
			//bbidxが0なら0からMaxまで、1ならMaxからMax*2までを格納するそれ以外ならskip
			if (i / MultiRenderTargets::Max != bbidx)continue;
			rtvs.push_back(*renderTargetView_[i]);
			rts.push_back(*renderTargetTexture_[i]);
		//レンダーターゲットとしてバリアを張る
			cmdList->TransitionBarrier(renderTargetTexture_[i].get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		}


		if (createDepthStencil_)
		renderTargetManager->PushRenderTarget(rtvs, rts, depthStencilView_.get(), depthStencilTexture_.get(), true);
		else
			renderTargetManager->PushRenderTarget(rtvs, rts, nullptr, nullptr, true);
		sceneRenderRegistrar->Draw();	

		renderTargetManager->PopRenderTarget();

		for (int i = 0; i < renderTargetTextureView_.size(); i++)
		{
			//bbidxが0なら0からMaxまで、1ならMaxからMax*2までを格納するそれ以外ならskip
			if (i / MultiRenderTargets::Max != bbidx)continue;
			//rtvs.push_back(*renderTargetView_[i]);
			//rts.push_back(*renderTargetTexture_[i]);
			//テクスチャとしてバリアを張る
			cmdList->TransitionBarrier(renderTargetTexture_[i].get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		cManager.SetMainCamera(prvCamera->name_);

		rtvs.clear();
		rts.clear();
		now_ = nullptr;
		bbidx = (bbidx + 1) % SwapChain::kFrameCount;//次のバックバッファのインデックスをセット
		return true;
	}

	bool RenderTexture::Draw()
	{
		if (!isCreated_)return false;

		auto renderTargetManager = ym::RenderTargetManager::Instance();
		auto sceneRenderRegistrar = ym::SceneRenderRegistrar::Instance();
		auto &cManager = CameraManager::Instance();
		auto prvCamera = cManager.GetMainCamera();
		//bbidx = Renderer::Instance()->GetDevice()->GetSwapChain().GetFrameIndex();

		auto cmdList = Renderer::Instance()->GetGraphicCommandList();

		cManager.SetMainCamera(useCameraName_);
		vector<RenderTargetView>rtvs;
		vector<Texture>rts;
		for (int i = 0; i < renderTargetTextureView_.size(); i++)
		{
			//bbidxが0なら0からMaxまで、1ならMaxからMax*2までを格納するそれ以外ならskip
			if (i / MultiRenderTargets::Max != bbidx)continue;
			rtvs.push_back(*renderTargetView_[i]);
			rts.push_back(*renderTargetTexture_[i]);
			//レンダーターゲットとしてバリアを張る
			cmdList->TransitionBarrier(renderTargetTexture_[i].get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		}


		if (createDepthStencil_)
			renderTargetManager->PushRenderTarget(rtvs, rts, depthStencilView_.get(), depthStencilTexture_.get(), true);
		else
			renderTargetManager->PushRenderTarget(rtvs, rts, nullptr, nullptr, true);
		sceneRenderRegistrar->Draw();

		renderTargetManager->PopRenderTarget();

		for (int i = 0; i < renderTargetTextureView_.size(); i++)
		{
			//bbidxが0なら0からMaxまで、1ならMaxからMax*2までを格納するそれ以外ならskip
			if (i / MultiRenderTargets::Max != bbidx)continue;
			//rtvs.push_back(*renderTargetView_[i]);
			//rts.push_back(*renderTargetTexture_[i]);
			//テクスチャとしてバリアを張る
			cmdList->TransitionBarrier(renderTargetTexture_[i].get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		cManager.SetMainCamera(prvCamera->name_);

		rtvs.clear();
		rts.clear();
		bbidx = (bbidx + 1) % SwapChain::kFrameCount;//次のバックバッファのインデックスをセット
		return true;
	}

	void RenderTexture::Uninit()
	{

		for (auto &texture : renderTargetTexture_)
		{
			texture->Uninit();
		}
		for (auto &view : renderTargetView_)
		{
			view->Uninit();
		}
		for (auto &view : renderTargetTextureView_)
		{
			view->Destroy();
		}
		if (createDepthStencil_)
		{
			depthStencilTexture_->Uninit();
			depthStencilView_->Uninit();
		}
		renderTargetTexture_.clear();
		renderTargetView_.clear();
		renderTargetTextureView_.clear();
		depthStencilTexture_.reset();
		depthStencilView_.reset();

	}
	void RenderTexture::Create(u32 width, u32 height, bool createDepthStencil)
	{

		CreateRenderTargets(width, height);
		if (createDepthStencil)
		{
			CreateDepthStencil(width, height);
		}
		createDepthStencil_ = createDepthStencil;
		isCreated_ = true;
	}
	void RenderTexture::CreateRenderTargets(u32 width, u32 height)
	{
		auto device = ym::Renderer::Instance()->GetDevice();
		int createCount = ym::SwapChain::kFrameCount * MultiRenderTargets::Max;

		renderTargetTexture_.resize(createCount);
		renderTargetView_.resize(createCount);
		renderTargetTextureView_.resize(createCount);
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
		for (int i = 0; i < createCount; i++)
		{
			renderTargetTexture_[i] = std::make_shared<ym::Texture>();
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
			renderTargetTexture_[i]->Init(device, texDesc);
			renderTargetView_[i] = std::make_shared<ym::RenderTargetView>();
			renderTargetView_[i]->Init(device,renderTargetTexture_[i].get());
			renderTargetTextureView_[i] = std::make_shared<ym::TextureView>();
			renderTargetTextureView_[i]->Init(device,renderTargetTexture_[i].get());
		}

	}
	void RenderTexture::CreateDepthStencil(u32 width, u32 height)
	{
		auto device = ym::Renderer::Instance()->GetDevice();
		ym::TextureDesc texDesc;
		texDesc.dimension = TextureDimension::Texture2D;
		texDesc.width = width;
		texDesc.height = height;
		texDesc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.isDepthBuffer = true;
		texDesc.clearDepth = 1.0f;
		texDesc.clearStencil = 0;
		depthStencilTexture_->Init(device, texDesc);
		depthStencilView_->Init(device, depthStencilTexture_.get());
	}
}