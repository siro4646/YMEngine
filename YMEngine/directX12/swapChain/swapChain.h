#pragma once

#include "texture/texture.h"
#include "textureView/textureView.h"

namespace ym
{
	class Device;
	class CommandQueue;

	class SwapChain
	{
	public:
		static const u32 kFrameCount = 2;
		SwapChain()
		{}
		~SwapChain()
		{
			Uninit();
		}
		bool Init(Device *pDev, CommandQueue *pQueue, HWND hwnd, u32 width, u32 height);
		void Uninit();

		void Present(int syncInterval);

		void Resize(u32 width, u32 height);
		void Resize(HWND hwnd);
		Vector2 GetSize() const
		{
			return Vector2(width_, height_);
		}

		//Getter
		IDXGISwapChain4 *GetSwapChainDep()
		{
			return pSwapChain_.Get();
		}
		u32 GetFrameIndex() const
		{
			return frameIndex_;
		}
		Texture *GetTexture(int index) { return &backBufferTextures[index% kFrameCount]; }
		Texture *GetCurrentTexture(int offset = 0) { return &backBufferTextures[(frameIndex_ + offset) % kFrameCount]; }
		RenderTargetView *GetRenderTargetView(int index) { return &renderTargetViews_[index% kFrameCount]; }
		RenderTargetView *GetCurrentRenderTargetView(int offset = 0) { return &renderTargetViews_[(frameIndex_ + offset) % kFrameCount]; }
		TextureView *GetTextureView(int index) { return &sceneRenderTargetTexViews_[index % kFrameCount]; }
		TextureView *GetCurrentTextureView(int offset = 0) { return &sceneRenderTargetTexViews_[(frameIndex_ + offset) % kFrameCount]; }

		Texture *GetDepthStencilTexture() { return &depthStencilTexture_; }
		DepthStencilView *GetDepthStencilView() { return &depthStencilView_; }
		TextureView *GetDepthStencilTexView() { return &depthStencilTexView_; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescHandle(int index);



	private:
		Device *pDevice_{ nullptr };
		ComPtr<IDXGISwapChain4> pSwapChain_{ nullptr };

		//レンダーターゲット
		Texture backBufferTextures[kFrameCount];
		RenderTargetView		renderTargetViews_[kFrameCount];
		TextureView sceneRenderTargetTexViews_[kFrameCount];
		//深度ステンシル
		Texture depthStencilTexture_;
		DepthStencilView depthStencilView_;
		TextureView depthStencilTexView_;
		


		u32 frameIndex_{ 0 };
		u32 width_{ 0 };
		u32 height_{ 0 };

	};
	

}