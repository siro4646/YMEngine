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

		//Getter
		IDXGISwapChain4 *GetSwapChainDep()
		{
			return pSwapChain_.Get();
		}
		u32 GetFrameIndex() const
		{
			return frameIndex_;
		}
		Texture *GetTexture(int index) { return &textures_[index% kFrameCount]; }
		Texture *GetCurrentTexture(int offset = 0) { return &textures_[(frameIndex_ + offset) % kFrameCount]; }
		RenderTargetView *GetRenderTargetView(int index) { return &views_[index% kFrameCount]; }
		RenderTargetView *GetCurrentRenderTargetView(int offset = 0) { return &views_[(frameIndex_ + offset) % kFrameCount]; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescHandle(int index);



	private:
		ComPtr<IDXGISwapChain4> pSwapChain_{ nullptr };

		Texture textures_[kFrameCount];
		RenderTargetView		views_[kFrameCount];


		u32 frameIndex_{ 0 };
		u32 width_{ 0 };
		u32 height_{ 0 };

	};
	

}