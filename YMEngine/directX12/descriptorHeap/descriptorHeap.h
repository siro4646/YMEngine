#pragma once

namespace ym
{


	class Device;
	class Descriptor;
	class CommandList;

	class DescriptorHeap
	{
	public:
		DescriptorHeap()
		{}
		~DescriptorHeap()
		{
			Destroy();
		}

		bool Initialize(Device *pDev, const D3D12_DESCRIPTOR_HEAP_DESC &desc);
		void Destroy();

		Descriptor *CreateDescriptor();
		void ReleaseDescriptor(Descriptor *p);

		// getter
		ID3D12DescriptorHeap *GetHeap() { return pHeap_; }

	private:
		ID3D12DescriptorHeap *pHeap_{ nullptr };
		Descriptor *pDescriptors_{ nullptr };
		//Descriptor*					pUsedList_{ nullptr };
		Descriptor *pUnusedList_{ nullptr };
		D3D12_DESCRIPTOR_HEAP_DESC	heapDesc_{};
		uint32_t					descSize_{ 0 };
		uint32_t					take_num_{ 0 };
	};	// class DescriptorHeap


	class DescriptorAllocator;

	struct DescriptorInfo
	{
		DescriptorAllocator *pAllocator = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE	cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE	gpuHandle;
		u32							index = 0;

		bool IsValid() const
		{
			return pAllocator != nullptr;
		}

		void Free();
	};	// struct DescriptorInfo

	class DescriptorAllocator
	{
	public:
		DescriptorAllocator()
		{}
		~DescriptorAllocator()
		{
			Destroy();
		}

		bool Initialize(Device *pDev, const D3D12_DESCRIPTOR_HEAP_DESC &desc);
		void Destroy();

		DescriptorInfo Allocate();
		void Free(DescriptorInfo info);

	private:
		std::mutex					mutex_;
		ID3D12DescriptorHeap *pHeap_ = nullptr;
		D3D12_DESCRIPTOR_HEAP_DESC	heapDesc_{};
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleStart_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleStart_;
		u8 *pUseFlags_ = nullptr;
		u32							descSize_ = 0;
		u32							allocCount_ = 0;
		u32							currentPosition_ = 0;
	};	// class DescriptorAllocator

}	// namespace ym