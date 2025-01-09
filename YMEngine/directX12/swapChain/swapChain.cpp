#include "swapChain.h"
#include "device/device.h"
#include "commandQueue/commandQueue.h"

namespace ym
{

	bool SwapChain::Init(Device *pDev, CommandQueue *pQueue, HWND hwnd, u32 width, u32 height)
	{
		bool enableHDR = pDev->GetColorSpaceType() != ColorSpaceType::Rec709;

		{
			DXGI_SWAP_CHAIN_DESC1 desc = {};
			desc.BufferCount = kFrameCount;
			desc.Width = width;
			desc.Height = height;
			desc.Format = enableHDR ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc.SampleDesc.Count = 1;
			desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

			IDXGISwapChain1 *pSwap;
			auto hr = pDev->GetFactoryDep()->CreateSwapChainForHwnd(pQueue->GetQueueDep(), hwnd, &desc, nullptr, nullptr, &pSwap);
			if (FAILED(hr))
			{
				return false;
			}

			hr = pSwap->QueryInterface(IID_PPV_ARGS(&pSwapChain_));
			if (FAILED(hr))
			{
				return false;
			}

			const float kChromaticityLevel = 50000.0f;
			const float kLuminanceLevel = 10000.0f;
			DXGI_HDR_METADATA_HDR10 MetaData;
			switch (pDev->GetColorSpaceType())
			{
			case ColorSpaceType::Rec709:
				pSwapChain_->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709);
				MetaData.RedPrimary[0] = (UINT16)(0.640f * kChromaticityLevel);
				MetaData.RedPrimary[1] = (UINT16)(0.330f * kChromaticityLevel);
				MetaData.GreenPrimary[0] = (UINT16)(0.300f * kChromaticityLevel);
				MetaData.GreenPrimary[1] = (UINT16)(0.600f * kChromaticityLevel);
				MetaData.BluePrimary[0] = (UINT16)(0.150f * kChromaticityLevel);
				MetaData.BluePrimary[1] = (UINT16)(0.060f * kChromaticityLevel);
				MetaData.WhitePoint[0] = (UINT16)(0.3127f * kChromaticityLevel);
				MetaData.WhitePoint[1] = (UINT16)(0.3290f * kChromaticityLevel);
				MetaData.MaxMasteringLuminance = (UINT)(pDev->GetMaxLuminance() * kLuminanceLevel);
				MetaData.MinMasteringLuminance = (UINT)(pDev->GetMinLuminance() * kLuminanceLevel);
				MetaData.MaxContentLightLevel = (UINT16)(100.0f);
				pSwapChain_->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(MetaData), &MetaData);
				break;
			case ColorSpaceType::Rec2020:
				pSwapChain_->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
				MetaData.RedPrimary[0] = (UINT16)(0.708f * kChromaticityLevel);
				MetaData.RedPrimary[1] = (UINT16)(0.292f * kChromaticityLevel);
				MetaData.GreenPrimary[0] = (UINT16)(0.170f * kChromaticityLevel);
				MetaData.GreenPrimary[1] = (UINT16)(0.797f * kChromaticityLevel);
				MetaData.BluePrimary[0] = (UINT16)(0.131f * kChromaticityLevel);
				MetaData.BluePrimary[1] = (UINT16)(0.046f * kChromaticityLevel);
				MetaData.WhitePoint[0] = (UINT16)(0.3127f * kChromaticityLevel);
				MetaData.WhitePoint[1] = (UINT16)(0.3290f * kChromaticityLevel);
				MetaData.MaxMasteringLuminance = (UINT)(pDev->GetMaxLuminance() * kLuminanceLevel);
				MetaData.MinMasteringLuminance = (UINT)(pDev->GetMinLuminance() * kLuminanceLevel);
				MetaData.MaxContentLightLevel = (UINT16)(2000.0f);
				MetaData.MaxFrameAverageLightLevel = (UINT16)(pDev->GetMaxFullFrameLuminance());
				pSwapChain_->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(MetaData), &MetaData);
				break;
			}

			frameIndex_ = pSwapChain_->GetCurrentBackBufferIndex();
			width_ = width;
			height_ = height;
			pDevice_ = pDev;
			//swapchainEvent_ = pSwapChain_->GetFrameLatencyWaitableObject();

			pSwap->Release();
		}

		//テクスチャの初期化
		{
			for (u32 i = 0; i < kFrameCount; ++i)
			{
				if (!backBufferTextures[i].InitFromSwapChain(pDev, this, i))
				{
					return false;
				}
				if (!renderTargetViews_[i].Init(pDev, &backBufferTextures[i]))
				{
					return false;
				}
				if (!sceneRenderTargetTexViews_[i].Init(pDev, &backBufferTextures[i]))
				{
					return false;
				}
			}

		}
		{
			ym::TextureDesc texDesc;
			texDesc.dimension = TextureDimension::Texture2D;
			texDesc.width = width;
			texDesc.height = height;
			texDesc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			texDesc.isDepthBuffer = true;
			texDesc.clearDepth = 1.0f;
			texDesc.clearStencil = 0;
			//texDesc.

			if (!depthStencilTexture_.Init(pDev, texDesc))
			{
				return false;
			}
			if (!depthStencilView_.Init(pDev, &depthStencilTexture_))
			{
				return false;
			}
			if (!depthStencilTexView_.Init(pDev, &depthStencilTexture_))
			{
				return false;
			}

		}

		return true;

	}
	void SwapChain::Uninit()
	{
		ym::ConsoleLog("[INFO]SwapChain::Uninit()\n");

		//レンダーターゲット
		for (auto v : renderTargetViews_)
		{
			v.Uninit();
		}
		for (auto v : sceneRenderTargetTexViews_)
		{
			v.Destroy();
		}
		for (auto t : backBufferTextures)
		{
			t.Uninit();
		}
		//深度ステンシル
		depthStencilView_.Uninit();
		depthStencilTexView_.Destroy();
		depthStencilTexture_.Uninit();

		pSwapChain_.Reset();

	}
	void SwapChain::Present(int syncInterval)
	{
		pSwapChain_->Present(syncInterval, 0);
		frameIndex_ = pSwapChain_->GetCurrentBackBufferIndex();
	}
	void SwapChain::Resize(u32 width, u32 height)
	{
		pSwapChain_->ResizeBuffers(kFrameCount, width, height, pDevice_->GetSwapChain().GetCurrentRenderTargetView()->GetFormat(), 0);
		width_ = width;
		height_ = height;

	}
	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetDescHandle(int index)
	{
		return renderTargetViews_[index].GetDescInfo().cpuHandle;
	}
}
