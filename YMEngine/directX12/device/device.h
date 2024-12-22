#pragma once

#include "descriptorHeap/descriptorHeap.h"


namespace ym
{

	class CommandQueue;
	class SwapChain;
	class DescriptorAllocator;

	class Device
	{
	public:
		Device();


		~Device();
		bool Init(HWND hwnd, u32 width, u32 height, ColorSpaceType csType = ColorSpaceType::Rec709);
		void Uninit();

		void WaitForCommandQueue();

		//Getter
		IDXGIFactory4 *GetFactoryDep()
		{
			return pFactory_.Get();
		}
		ID3D12Device *GetDeviceDep()
		{
			return pDevice_.Get();
		}
		CommandQueue &GetGraphicsQueue()
		{
			return *pGraphicsQueue_;
		}

		ColorSpaceType	GetColorSpaceType() const
		{
			return colorSpaceType_;
		}
		RECT			GetDesktopCoordinates() const
		{
			return desktopCoordinates_;
		}
		float			GetMinLuminance() const
		{
			return minLuminance_;
		}
		float			GetMaxLuminance() const
		{
			return maxLuminance_;
		}
		float			GetMaxFullFrameLuminance() const
		{
			return maxFullFrameLuminance_;
		}

		DescriptorAllocator &GetRtvDescriptorHeap()
		{
			return *pRtvDescHeap_;
		}

		SwapChain &GetSwapChain()
		{
			return *pSwapChain_;
		}
		void Present(int syncInterval = 1);

	private:
		bool CreateFactory();

		bool CreateDevice(ColorSpaceType);

		bool CreateCommandQueue();

		bool CreateFence();

		bool CreateSwapChain(HWND hwnd, u32 width, u32 height);

		bool CreateDescriptorHeap();

	private:
		ComPtr<IDXGIFactory7> pFactory_;
		ComPtr<ID3D12Device> pDevice_;

		std::unique_ptr<CommandQueue> pGraphicsQueue_{ nullptr };
		std::unique_ptr<SwapChain> pSwapChain_{ nullptr };

		ComPtr<ID3D12Fence> pFence_{ nullptr };
		u32				fenceValue_{ 0 };
		HANDLE			fenceEvent_{ nullptr };

		//DescriptorHeap
		DescriptorAllocator *pRtvDescHeap_ = nullptr;



		ColorSpaceType	colorSpaceType_ = ColorSpaceType::Rec709;
		RECT			desktopCoordinates_;
		float			minLuminance_ = 0.0f;
		float			maxLuminance_ = 0.0f;
		float			maxFullFrameLuminance_ = 0.0f;


	};
}