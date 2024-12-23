#include "renderer.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"
#include "swapChain/swapChain.h"

namespace ym
{

	bool Renderer::Init(HWND hwnd, u32 width, u32 height, ColorSpaceType csType)
	{
		ym::ConsoleLog("Renderer::Init\n");
		device_ = std::make_shared<Device>();
		if (!device_)
		{
			return false;
		}

		if (!device_->Init(hwnd, width, height, csType))
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

		
		return true;
	}
	void Renderer::Uninit()
	{
		device_->WaitForCommandQueue();

		commandList_->Uninit();

		device_->Uninit();



	}
	void Renderer::BeginFrame()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto nextbbidx = (bbidx + 1) % ym::SwapChain::kFrameCount;
		auto cmd = commandList_;

		auto scTex = device_->GetSwapChain().GetCurrentTexture();
		cmd->TransitionBarrier(scTex, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		float clearColor[] = { 0.5,1,1,1 };
		auto rtv = device_->GetSwapChain().GetDescHandle(bbidx);
		cmd->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
		cmd->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		
		SetViewPort();


	}
	void Renderer::EndFrame()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmd = commandList_;
		auto scTex = device_->GetSwapChain().GetCurrentTexture();
		cmd->TransitionBarrier(scTex, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


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
}
