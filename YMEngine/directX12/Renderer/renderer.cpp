#include "renderer.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"
#include "swapChain/swapChain.h"
#include "winAPI/window/window.h"

#include "camera/cameraManager.h"
#include "camera/camera.h"


namespace ym
{
	std::shared_ptr<Device> Renderer::device_{};
	std::shared_ptr<CommandList> Renderer::commandList_{};

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
		commandList_ = std::make_shared<CommandList>();
		if (!commandList_)
		{
			return false;
		}
		if (!commandList_->Init(device_.get(),&device_->GetGraphicsQueue()))
		{
			return false;
		}		
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

		//_polygon.Init(device_.get(), commandList_.get());

		auto &cameraManager = CameraManager::Instance();
		cameraManager.CreateCamera("mainCamera");
		CreateGraphicsItem();

		return true;
	}
	void Renderer::Uninit()
	{
		device_->WaitForCommandQueue();

		if (window_)
		{
			window_ = nullptr;
		}
		

		_polygon.Destroy();

		commandList_->Uninit();

		commandList_.reset();

		device_->Uninit();

		device_.reset();
	}
	void Renderer::Update()
	{
		//_polygon.Update();
	}
	void Renderer::Resize(u32 width, u32 height)
	{
		device_->WaitForCommandQueue();
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
		auto cmd = commandList_;

		auto scTex = device_->GetSwapChain().GetCurrentTexture();
		auto rtv = device_->GetSwapChain().GetDescHandle(bbidx);
		auto dsv = device_->GetSwapChain().GetDepthStencilView()->GetDescInfo().cpuHandle;
		auto dsvTex = device_->GetSwapChain().GetDepthStencilTexture();

		cmd->TransitionBarrier(scTex,D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmd->TransitionBarrier(dsvTex,D3D12_RESOURCE_STATE_DEPTH_WRITE);
		float clearColor[] = { 0.5,1,1,1 };

		cmd->GetCommandList()->OMSetRenderTargets(1, &rtv, true, &dsv);
		cmd->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		cmd->GetCommandList()->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH,dsvTex->GetTextureDesc().clearDepth,dsvTex->GetTextureDesc().clearStencil, 0, nullptr);
		
		SetViewPort();

	}
	void Renderer::Draw()
	{
		
	}
	void Renderer::EndFrame()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmd = commandList_;
		auto scTex = device_->GetSwapChain().GetCurrentTexture();
		auto dsvTex = device_->GetSwapChain().GetDepthStencilTexture();
		cmd->TransitionBarrier(scTex, D3D12_RESOURCE_STATE_PRESENT);
		cmd->TransitionBarrier(dsvTex,  D3D12_RESOURCE_STATE_GENERIC_READ);

		commandList_->Close();
		commandList_->Execute();
		device_->WaitForCommandQueue();
		cmd->Reset();
		device_->Present();
	}
	void Renderer::SetViewPort()
	{
		auto cmd = commandList_;
		cmd->GetCommandList()->RSSetViewports(1, &m_viewPort);
		cmd->GetCommandList()->RSSetScissorRects(1, &m_scissorRect);
	}
	void Renderer::CreateGraphicsItem()
	{

	}
}
