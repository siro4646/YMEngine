
#include "descriptorHeap.h"
#include "device/device.h"
#include "descriptor/descriptor.h"
#include "swapChain/swapChain.h"
#include "commandList/commandList.h"


namespace ym
{
	bool DescriptorHeap::Initialize(Device *pDev, const D3D12_DESCRIPTOR_HEAP_DESC &desc)
	{
		auto hr = pDev->GetDeviceDep()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pHeap_));
		if (FAILED(hr))
		{
			return false;
		}

		pDescriptors_ = new Descriptor[desc.NumDescriptors + 2];
		//pUsedList_ = pDescriptors_;
		//pUsedList_->pPrev_ = pUsedList_->pNext_ = pUsedList_;
		pUnusedList_ = pDescriptors_ + 1;
		pUnusedList_->pPrev_ = pUnusedList_->pNext_ = pUnusedList_;

		heapDesc_ = desc;
		descSize_ = pDev->GetDeviceDep()->GetDescriptorHandleIncrementSize(desc.Type);

		// ‚·‚×‚ÄUnusedList‚ÉÚ‘±
		Descriptor *p = pUnusedList_ + 1;
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu = pHeap_->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE hGpu = pHeap_->GetGPUDescriptorHandleForHeapStart();
		for (u32 i = 0; i < desc.NumDescriptors; i++, p++, hCpu.ptr += descSize_, hGpu.ptr += descSize_)
		{
			p->pParentHeap_ = this;
			p->cpuHandle_ = hCpu;
			p->gpuHandle_ = hGpu;
			p->index_ = i;

			Descriptor *next = pUnusedList_->pNext_;
			pUnusedList_->pNext_ = next->pPrev_ = p;
			p->pPrev_ = pUnusedList_;
			p->pNext_ = next;
		}

		take_num_ = 0;

		return true;
	}
	//----
	void DescriptorHeap::Destroy()
	{
		//assert(pUsedList_->pNext_ == pUsedList_);
		delete[] pDescriptors_;
		SafeRelease(pHeap_);
	}

	//----
	Descriptor *DescriptorHeap::CreateDescriptor()
	{
		if (pUnusedList_->pNext_ == pUnusedList_)
		{
			return nullptr;
		}

		Descriptor *ret = pUnusedList_->pNext_;
		ret->pPrev_->pNext_ = ret->pNext_;
		ret->pNext_->pPrev_ = ret->pPrev_;
		ret->pNext_ = ret->pPrev_ = ret;
		take_num_++;

		return ret;
	}

	//----
	void DescriptorHeap::ReleaseDescriptor(Descriptor *p)
	{
		assert((pDescriptors_ <= p) && (p <= pDescriptors_ + heapDesc_.NumDescriptors + 2));

		Descriptor *next = pUnusedList_->pNext_;
		pUnusedList_->pNext_ = next->pPrev_ = p;
		p->pPrev_ = pUnusedList_;
		p->pNext_ = next;
		take_num_--;
	}

	//-----------------------------------------

	//----
	void DescriptorInfo::Free()
	{
		if (IsValid())
		{
			pAllocator->Free(*this);
			pAllocator = nullptr;
		}
	}
	bool DescriptorAllocator::Initialize(Device *pDev, const D3D12_DESCRIPTOR_HEAP_DESC &desc)
	{
		auto hr = pDev->GetDeviceDep()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pHeap_));
		if (FAILED(hr))
		{
			return false;
		}

		pUseFlags_ = new u8[desc.NumDescriptors];
		memset(pUseFlags_, 0, sizeof(u8) * desc.NumDescriptors);

		cpuHandleStart_ = pHeap_->GetCPUDescriptorHandleForHeapStart();
		if (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			gpuHandleStart_ = pHeap_->GetGPUDescriptorHandleForHeapStart();
		}
		else
		{
			gpuHandleStart_.ptr = 0L;
		}

		heapDesc_ = desc;
		descSize_ = pDev->GetDeviceDep()->GetDescriptorHandleIncrementSize(desc.Type);
		allocCount_ = 0;
		currentPosition_ = 0;

		return true;
	}

	//----
	void DescriptorAllocator::Destroy()
	{
		assert(allocCount_ == 0);

		SafeRelease(pHeap_);
		SafeDeleteArray(pUseFlags_);
	}

	//----
	DescriptorInfo DescriptorAllocator::Allocate()
	{
		DescriptorInfo ret;

		std::lock_guard<std::mutex> lock(mutex_);

		if (allocCount_ == heapDesc_.NumDescriptors)
			return ret;

		auto cp = currentPosition_;
		for (u32 i = 0; i < heapDesc_.NumDescriptors; i++, cp++)
		{
			cp = cp % heapDesc_.NumDescriptors;
			if (!pUseFlags_[cp])
			{
				pUseFlags_[cp] = 1;
				ret.pAllocator = this;
				ret.cpuHandle = cpuHandleStart_;
				ret.gpuHandle = gpuHandleStart_;
				ret.cpuHandle.ptr += descSize_ * cp;
				ret.gpuHandle.ptr += descSize_ * cp;
				ret.index = cp;

				currentPosition_ = cp + 1;
				allocCount_++;
				break;
			}
		}

		return ret;
	}

	//----
	void DescriptorAllocator::Free(DescriptorInfo info)
	{
		assert(info.pAllocator == this);
		assert(pUseFlags_[info.index] != 0);

		std::lock_guard<std::mutex> lock(mutex_);
		pUseFlags_[info.index] = 0;
		allocCount_--;
	}
}
