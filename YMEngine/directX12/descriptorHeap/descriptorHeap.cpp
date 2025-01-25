
#include "descriptorHeap.h"
#include "device/device.h"
#include "descriptor/descriptor.h"
#include "swapChain/swapChain.h"
#include "commandList/commandList.h"
#include "descriptorSet/DescriptorSet.h"


namespace ym
{
	namespace
	{
		const u32	kMaxSamplerDesc = 2048;		// Sampler�̍ő吔
	}

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

		// ���ׂ�UnusedList�ɐڑ�
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
			if (DEBUG_DESCRIPTOR_HEAP)
			{
				ym::ConsoleLog("[INFO] DescriptorInfo::Free() called\n");
				ym::ConsoleLog("[INFO] pAllocator = %d\n", pAllocator->allocCount_);
			}
			if (pAllocator->allocCount_ == 0)
			{
				if (DEBUG_DESCRIPTOR_HEAP)
				ym::ConsoleLog("[INFO] DescriptorInfo::Free() pAllocator->allocCount_ == 0\n");
				//pAllocator = nullptr;
			}
			else
			{
				pAllocator->Free(*this);
				pAllocator = nullptr;
			}
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
		if (DEBUG_DESCRIPTOR_HEAP)
		{
			ym::ConsoleLog("[INFO] DescriptorAllocator::Destroy() called\n");
			ym::ConsoleLog("[INFO] allocCount_ = %d\n", allocCount_);
		}
		//0����Ȃ��Ȃ�J�����Ȃ�
		if (allocCount_ != 0) return;


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
		if (DEBUG_DESCRIPTOR_HEAP) {
			ym::ConsoleLog("[INFO] DescriptorAllocator::Free() called\n");
		}
		//assert(info.pAllocator == this);
		//assert(pUseFlags_[info.index] != 0);
		if (info.pAllocator == this && pUseFlags_[info.index] != 0)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			pUseFlags_[info.index] = 0;
			allocCount_--;
		}

		/*std::lock_guard<std::mutex> lock(mutex_);
		pUseFlags_[info.index] = 0;
		allocCount_--;*/

	}

	bool DescriptorStack::Allocate(u32 count, D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle)
	{
		if (stackPosition_ + count > stackMax_)
			return false;

		u32 prev = stackPosition_;
		stackPosition_ += count;

		cpuHandle = cpuHandleStart_;
		cpuHandle.ptr += prev * descSize_;
		gpuHandle = gpuHandleStart_;
		gpuHandle.ptr += prev * descSize_;

		return true;
	}

	//----
	bool DescriptorStackList::Initialilze(GlobalDescriptorHeap *parent)
	{
		pParentHeap_ = parent;
		stackIndex_ = 0;

		return AddStack();
	}

	//----
	void DescriptorStackList::Allocate(u32 count, D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle)
	{
		if (stacks_[stackIndex_].Allocate(count, cpuHandle, gpuHandle))
			return;

		stackIndex_++;
		if (stacks_.size() <= stackIndex_)
		{
			if (!AddStack())
				assert(!"[ERROR] Stack Empty!!");
		}

		if (!stacks_[stackIndex_].Allocate(count, cpuHandle, gpuHandle))
			assert(!"[ERROR] Stack Empty!!");
	}

	//----
	void DescriptorStackList::Reset()
	{
		for (auto &&stack : stacks_) stack.Reset();
		stackIndex_ = 0;
	}

	void DescriptorStackList::Destroy()
	{
		stacks_.clear();
		pParentHeap_ = nullptr;
	}

	//----
	bool DescriptorStackList::AddStack()
	{
		DescriptorStack stack;
		stacks_.push_back(stack);

		auto &&s = stacks_.back();
		return pParentHeap_->AllocateStack(s, 2000);
	}

	//----
	bool GlobalDescriptorHeap::Initialize(Device *pDev, const D3D12_DESCRIPTOR_HEAP_DESC &desc)
	{
		auto hr = pDev->GetDeviceDep()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pHeap_));
		if (FAILED(hr))
		{
			return false;
		}

		cpuHandleStart_ = pHeap_->GetCPUDescriptorHandleForHeapStart();
		gpuHandleStart_ = pHeap_->GetGPUDescriptorHandleForHeapStart();

		heapDesc_ = desc;
		descSize_ = pDev->GetDeviceDep()->GetDescriptorHandleIncrementSize(desc.Type);
		allocCount_ = 0;

		return true;
	}

	//----
	void GlobalDescriptorHeap::Destroy()
	{
		SafeRelease(pHeap_);
	}

	//----
	bool GlobalDescriptorHeap::AllocateStack(DescriptorStack &stack, u32 count)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if (allocCount_ + count > heapDesc_.NumDescriptors)
			return false;

		stack.cpuHandleStart_ = cpuHandleStart_;
		stack.cpuHandleStart_.ptr += descSize_ * allocCount_;
		stack.gpuHandleStart_ = gpuHandleStart_;
		stack.gpuHandleStart_.ptr += descSize_ * allocCount_;
		stack.descSize_ = descSize_;
		stack.stackMax_ = count;
		stack.stackPosition_ = 0;

		allocCount_ += count;

		return true;
	}


	//----
	bool SamplerDescriptorHeap::Initialize(Device *pDev)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = kMaxSamplerDesc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 1;

		auto hr = pDev->GetDeviceDep()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pHeap_));
		if (FAILED(hr))
		{
			return false;
		}

		cpuHandleStart_ = pHeap_->GetCPUDescriptorHandleForHeapStart();
		gpuHandleStart_ = pHeap_->GetGPUDescriptorHandleForHeapStart();

		descSize_ = pDev->GetDeviceDep()->GetDescriptorHandleIncrementSize(desc.Type);
		allocCount_ = 0;

		return true;
	}

	//----
	void SamplerDescriptorHeap::Destroy()
	{
		SafeRelease(pHeap_);
	}

	//----
	bool SamplerDescriptorHeap::Allocate(u32 count, D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle)
	{
		
		if (allocCount_ + count > kMaxSamplerDesc)
			return false;

		cpuHandle = cpuHandleStart_;
		cpuHandle.ptr += descSize_ * allocCount_;
		gpuHandle = gpuHandleStart_;
		gpuHandle.ptr += descSize_ * allocCount_;

		allocCount_ += count;

		return true;
	}

	//----
	bool SamplerDescriptorCache::Initialize(Device *pDev)
	{
		pParentDevice_ = pDev;

		return AddHeap();
	}

	//----
	void SamplerDescriptorCache::Destroy()
	{
		heapList_.clear();
	}

	//----
	bool SamplerDescriptorCache::AddHeap()
	{
		auto p = new SamplerDescriptorHeap();
		if (!p->Initialize(pParentDevice_))
			return false;

		pCurrentHeap_ = p;
		heapList_.push_back(std::unique_ptr<SamplerDescriptorHeap>(p));
		return true;
	}

	//----
	bool SamplerDescriptorCache::AllocateAndCopy(u32 count, D3D12_CPU_DESCRIPTOR_HANDLE *cpuHandles, D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle)
	{
		// �L���b�V�������݂��邩�ǂ�������
		auto hash = CalcFnv1a32(cpuHandles, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * count);
		auto findIt = descCache_.find(hash);
		if (findIt != descCache_.end())
		{
			gpuHandle = findIt->second.gpuHandle;
			pLastAllocateHeap_ = findIt->second.pHeap;
			return true;
		}

		// �V����Descriptor���m��
		D3D12_CPU_DESCRIPTOR_HANDLE ch;
		if (!pCurrentHeap_->Allocate(count, ch, gpuHandle))
		{
			// �m�ۂł��Ȃ������̂ŐV����Heap���쐬
			if (!AddHeap())
				return false;

			if (!pCurrentHeap_->Allocate(count, ch, gpuHandle))
				return false;
		}

		// �m�ۂ���Descriptor�ɃR�s�[����
		pParentDevice_->GetDeviceDep()->CopyDescriptors(
			1, &ch, &count,
			count, cpuHandles, nullptr,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		// �L���b�V���ɕۑ����Ă���
		MapItem item;
		item.pHeap = pCurrentHeap_;
		item.cpuHandle = ch;
		item.gpuHandle = gpuHandle;
		descCache_[hash] = item;

		pLastAllocateHeap_ = pCurrentHeap_;

		return true;
	}
}
