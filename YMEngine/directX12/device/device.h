#pragma once

#include "descriptorHeap/descriptorHeap.h"
#include "utility/resourceRelease/resourceRelease.h"



namespace ym
{

#define LatestDevice ID3D12Device6

	class CommandQueue;
	class SwapChain;
	class GlobalDescriptorHeap;
	class DescriptorHeap;
	class DescriptorAllocator;
	class CommandList;

	class Device: public std::enable_shared_from_this<Device>
	{
	public:
		Device();


		~Device();
		bool Init(HWND hwnd, u32 width, u32 height, ColorSpaceType csType = ColorSpaceType::Rec709);
		void Uninit();

		void WaitForGraphicsCommandQueue();

		void WaitForComputeCommandQueue();

		void WaitForCopyCommandQueue();

		void SyncKillObjects(bool bForce = false)
		{
			if (!bForce)
			{
				deathList_.SyncKill();
			}
			else
			{
				deathList_.Destroy();
			}
		}
		void PendingKill(PendingKillItem *p)
		{
			deathList_.PendingKill(p);
		}
		template <typename T>
		void KillObject(T *p)
		{
			deathList_.KillObject<T>(p);
		}


		//Getter
		IDXGIFactory4 *GetFactoryDep()
		{
			return pFactory_.Get();
		}
		ID3D12Device *GetDeviceDep()
		{
			return pDevice_.Get();
		}
		LatestDevice *GetLatestDeviceDep()
		{
			return pLatestDevice_.Get();
		}

		CommandQueue &GetGraphicsQueue()
		{
			return *pGraphicsQueue_;
		}
		CommandQueue &GetComputeQueue()
		{
			return *pComputeQueue_;
		}
		CommandQueue &GetCopyQueue()
		{
			return *pCopyQueue_;
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
		GlobalDescriptorHeap &GetGlobalViewDescriptorHeap()
		{
			return *pGlobalViewDescHeap_;
		}

		DescriptorAllocator &GetViewDescriptorHeap()
		{
			return *pViewDescHeap_;
		}
		DescriptorAllocator &GetSamplerDescriptorHeap()
		{
			return *pSamplerDescHeap_;
		}
		DescriptorAllocator &GetRtvDescriptorHeap()
		{
			return *pRtvDescHeap_;
		}
		DescriptorAllocator &GetDsvDescriptorHeap()
		{
			return *pDsvDescHeap_;
		}

		DescriptorInfo &GetDefaultViewDescInfo()
		{
			return defaultViewDescInfo_;
		}
		DescriptorInfo &GetDefaultSamplerDescInfo()
		{
			return defaultSamplerDescInfo_;
		}
		SwapChain &GetSwapChain()
		{
			return *pSwapChain_;
		}
		void Present(int syncInterval = 1);

		void Resize(u32 width, u32 height);

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
		ComPtr<LatestDevice> pLatestDevice_;


		std::unique_ptr<CommandQueue> pGraphicsQueue_{ nullptr };
		std::unique_ptr<CommandQueue> pComputeQueue_{ nullptr };
		std::unique_ptr<CommandQueue> pCopyQueue_{ nullptr };


		std::unique_ptr<SwapChain> pSwapChain_{ nullptr };

		ComPtr<ID3D12Fence> pGraphicsFence_{ nullptr };
		u32				graphicsFenceValue_{ 0 };
		HANDLE			graphicsFenceEvent_{ nullptr };

		ComPtr<ID3D12Fence> pComputeFence_{ nullptr };
		u32				computeFenceValue_{ 0 };
		HANDLE			computeFenceEvent_{ nullptr };

		ComPtr<ID3D12Fence> pCopyFence_{ nullptr };
		u32				copyFenceValue_{ 0 };
		HANDLE			copyFenceEvent_{ nullptr };

		//DescriptorHeap
		GlobalDescriptorHeap *pGlobalViewDescHeap_ = nullptr;

		DescriptorAllocator *pRtvDescHeap_ = nullptr;
		DescriptorAllocator *pDsvDescHeap_ = nullptr;
		DescriptorAllocator *pViewDescHeap_ = nullptr;
		DescriptorAllocator *pSamplerDescHeap_ = nullptr;

		DescriptorInfo	defaultViewDescInfo_;
		DescriptorInfo	defaultSamplerDescInfo_;

		DeathList deathList_;


		ColorSpaceType	colorSpaceType_ = ColorSpaceType::Rec709;
		RECT			desktopCoordinates_;
		float			minLuminance_ = 0.0f;
		float			maxLuminance_ = 0.0f;
		float			maxFullFrameLuminance_ = 0.0f;

		bool isUninited = false;


	};
}