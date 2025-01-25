#include "commandList.h"
#include "device/device.h"
#include "commandQueue/commandQueue.h"
#include "texture/texture.h"
#include "buffer/buffer.h"
#include "descriptorHeap/descriptorHeap.h"
#include "descriptorSet/descriptorSet.h"
#include "rootSignature/rootSignature.h"

namespace ym
{
	void CommandList::TransitionBarrier(Texture *p, D3D12_RESOURCE_STATES nextState)
	{
		auto rST = ResourceStateTracker::Instance();
		auto prevState = rST->GetState(p->pResource_.Get());
		TransitionBarrier(p, prevState, nextState);
	}
	void CommandList::TransitionBarrier(Texture *p, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES nextState)
	{
		if (!p)
			return;

		if (prevState != nextState)
		{
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = p->pResource_.Get();
			barrier.Transition.StateBefore = prevState;
			barrier.Transition.StateAfter = nextState;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			GetCommandList()->ResourceBarrier(1, &barrier);
			auto rST = ResourceStateTracker::Instance();
			rST->SetState(p->pResource_.Get(), nextState);
		}
	}

	void CommandList::TransitionBarrier(Buffer *p, D3D12_RESOURCE_STATES nextState)
	{
		auto rST = ResourceStateTracker::Instance();
		auto prevState = rST->GetState(p->GetResourceDep());;
		TransitionBarrier(p, prevState, nextState);
	}

	void CommandList::TransitionBarrier(Buffer *p, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES nextState)
	{
		if (!p)
			return;

		if (prevState != nextState)
		{
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = p->pResource_;
			barrier.Transition.StateBefore = prevState;
			barrier.Transition.StateAfter = nextState;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			GetCommandList()->ResourceBarrier(1, &barrier);
			auto rST = ResourceStateTracker::Instance();
			rST->SetState(p->pResource_, nextState);
		}
	}

	void CommandList::UAVBarrier(Texture *p)
	{
		if (!p)
			return;

		auto rST = ResourceStateTracker::Instance();
		auto prevState = rST->GetState(p->pResource_.Get());
		if (prevState != D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.UAV.pResource = p->pResource_.Get();
			GetCommandList()->ResourceBarrier(1, &barrier);
			rST->SetState(p->pResource_.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		}
	}

	void CommandList::UAVBarrier(Buffer *p)
	{
		if (!p)
			return;

		auto rST = ResourceStateTracker::Instance();
		auto prevState = rST->GetState(p->pResource_);
		if (prevState != D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.UAV.pResource = p->pResource_;
			GetCommandList()->ResourceBarrier(1, &barrier);
			rST->SetState(p->pResource_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		}
	}

	void CommandList::SetGraphicsRootSignatureAndDescriptorSet(RootSignature *pRS, DescriptorSet *pDSet, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> **ppBindlessArrays)
	{

		auto pCmdList = GetCommandList();
		auto def_view = pDevice_->GetDefaultViewDescInfo().cpuHandle;
		auto def_sampler = pDevice_->GetDefaultSamplerDescInfo().cpuHandle;

		auto &&input_index = pRS->GetInputIndex();
		pCmdList->SetGraphicsRootSignature(pRS->GetRootSignature());

		// サンプラーステートのハンドルを確保する
		D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE sampler_handles[16 * 5];
		u32 sampler_count = 0;
		auto SetSamplerHandle = [&](const D3D12_CPU_DESCRIPTOR_HANDLE &handle)
			{
				sampler_handles[sampler_count++] = (handle.ptr > 0) ? handle : def_sampler;
			};
		for (u32 i = 0; i < pDSet->GetVsSampler().maxCount; i++)
		{
			SetSamplerHandle(pDSet->GetVsSampler().cpuHandles[i]);
		}
		for (u32 i = 0; i < pDSet->GetPsSampler().maxCount; i++)
		{
			SetSamplerHandle(pDSet->GetPsSampler().cpuHandles[i]);
		}
		for (u32 i = 0; i < pDSet->GetGsSampler().maxCount; i++)
		{
			SetSamplerHandle(pDSet->GetGsSampler().cpuHandles[i]);
		}
		for (u32 i = 0; i < pDSet->GetHsSampler().maxCount; i++)
		{
			SetSamplerHandle(pDSet->GetHsSampler().cpuHandles[i]);
		}
		for (u32 i = 0; i < pDSet->GetDsSampler().maxCount; i++)
		{
			SetSamplerHandle(pDSet->GetDsSampler().cpuHandles[i]);
		}
		if (sampler_count > 0)
		{
			bool isSuccess = pSamplerDescCache_->AllocateAndCopy(sampler_count, sampler_handles, samplerGpuHandle);
			assert(isSuccess);

			pCurrentSamplerHeap_ = pSamplerDescCache_->GetHeap();
			if (pCurrentSamplerHeap_ != pPrevSamplerHeap_)
			{
				pPrevSamplerHeap_ = pCurrentSamplerHeap_;
				changeHeap_ = true;
			}
		}

		// Heapが変更されている場合はここで設定し直す
		if (changeHeap_)
		{
			ID3D12DescriptorHeap *pDescHeaps[2];
			int heap_cnt = 0;
			if (pDevice_->GetGlobalViewDescriptorHeap().GetHeap())
				pDescHeaps[heap_cnt++] = pDevice_->GetGlobalViewDescriptorHeap().GetHeap();
			if (pCurrentSamplerHeap_)
				pDescHeaps[heap_cnt++] = pCurrentSamplerHeap_.Get();
			pCmdList->SetDescriptorHeaps(heap_cnt, pDescHeaps);
			changeHeap_ = false;
		}

		if (sampler_count > 0)
		{
			// サンプラーステートのハンドルを登録する
			auto sampler_desc_size = pDevice_->GetDeviceDep()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			auto SetSamplerDesc = [&](u32 count, u8 index)
				{
					if (count > 0)
					{
						pCmdList->SetGraphicsRootDescriptorTable(index, samplerGpuHandle);
						samplerGpuHandle.ptr += sampler_desc_size * count;
					}
				};
			SetSamplerDesc(pDSet->GetVsSampler().maxCount, input_index.vsSamplerIndex_);
			SetSamplerDesc(pDSet->GetPsSampler().maxCount, input_index.psSamplerIndex_);
			SetSamplerDesc(pDSet->GetGsSampler().maxCount, input_index.gsSamplerIndex_);
			SetSamplerDesc(pDSet->GetHsSampler().maxCount, input_index.hsSamplerIndex_);
			SetSamplerDesc(pDSet->GetDsSampler().maxCount, input_index.dsSamplerIndex_);
		}

		// CBV, SRV, UAVの登録
		auto SetViewDesc = [&](u32 count, const D3D12_CPU_DESCRIPTOR_HANDLE *handles, u8 index)
			{
				if (count > 0)
				{
					std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> tmp;
					tmp.resize(count);
					for (u32 i = 0; i < count; i++)
					{
						tmp[i] = (handles[i].ptr > 0) ? handles[i] : def_view;
					}

					D3D12_CPU_DESCRIPTOR_HANDLE dst_cpu;
					D3D12_GPU_DESCRIPTOR_HANDLE dst_gpu;
					pViewDescStack_->Allocate(count, dst_cpu, dst_gpu);
					pDevice_->GetDeviceDep()->CopyDescriptors(
						1, &dst_cpu, &count,
						count, tmp.data(), nullptr,
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					pCmdList->SetGraphicsRootDescriptorTable(index, dst_gpu);

					tmp.clear();
				}
			};
		SetViewDesc(pDSet->GetVsCbv().maxCount, pDSet->GetVsCbv().cpuHandles, input_index.vsCbvIndex_);
		SetViewDesc(pDSet->GetVsSrv().maxCount, pDSet->GetVsSrv().cpuHandles, input_index.vsSrvIndex_);
		SetViewDesc(pDSet->GetPsCbv().maxCount, pDSet->GetPsCbv().cpuHandles, input_index.psCbvIndex_);
		SetViewDesc(pDSet->GetPsSrv().maxCount, pDSet->GetPsSrv().cpuHandles, input_index.psSrvIndex_);
		SetViewDesc(pDSet->GetPsUav().maxCount, pDSet->GetPsUav().cpuHandles, input_index.psUavIndex_);
		SetViewDesc(pDSet->GetGsCbv().maxCount, pDSet->GetGsCbv().cpuHandles, input_index.gsCbvIndex_);
		SetViewDesc(pDSet->GetGsSrv().maxCount, pDSet->GetGsSrv().cpuHandles, input_index.gsSrvIndex_);
		SetViewDesc(pDSet->GetHsCbv().maxCount, pDSet->GetHsCbv().cpuHandles, input_index.hsCbvIndex_);
		SetViewDesc(pDSet->GetHsSrv().maxCount, pDSet->GetHsSrv().cpuHandles, input_index.hsSrvIndex_);
		SetViewDesc(pDSet->GetDsCbv().maxCount, pDSet->GetDsCbv().cpuHandles, input_index.dsCbvIndex_);
		SetViewDesc(pDSet->GetDsSrv().maxCount, pDSet->GetDsSrv().cpuHandles, input_index.dsSrvIndex_);

		// set bindless srv.
		auto bindless_infos = pRS->GetBindlessInfos();
		if (ppBindlessArrays && bindless_infos.size() != 0)
		{
			auto bindless_array_count = bindless_infos.size();
			for (size_t ba = 0; ba < bindless_array_count; ba++)
			{
				if (ppBindlessArrays[ba])
				{
					auto &&bindless = *ppBindlessArrays[ba];
					SetViewDesc((u32)bindless.size(), bindless.data(), bindless_infos[ba].index_);
				}
			}
		}
	}

	//----
	void CommandList::SetComputeRootSignatureAndDescriptorSet(RootSignature *pRS, DescriptorSet *pDSet, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> **ppBindlessArrays )
	{
		auto pCmdList = GetCommandList();
		auto def_view = pDevice_->GetDefaultViewDescInfo().cpuHandle;
		auto def_sampler = pDevice_->GetDefaultSamplerDescInfo().cpuHandle;

		auto &&input_index = pRS->GetInputIndex();
		pCmdList->SetComputeRootSignature(pRS->GetRootSignature());

		D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE sampler_handles[16];
		u32 sampler_count = 0;
		auto SetSamplerHandle = [&](const D3D12_CPU_DESCRIPTOR_HANDLE &handle)
			{
				sampler_handles[sampler_count++] = (handle.ptr > 0) ? handle : def_sampler;
			};
		for (u32 i = 0; i < pDSet->GetCsSampler().maxCount; i++)
		{
			SetSamplerHandle(pDSet->GetCsSampler().cpuHandles[i]);
		}
		if (sampler_count > 0)
		{
			bool isSuccess = pSamplerDescCache_->AllocateAndCopy(sampler_count, sampler_handles, samplerGpuHandle);
			assert(isSuccess);

			pCurrentSamplerHeap_ = pSamplerDescCache_->GetHeap();
			if (pCurrentSamplerHeap_ != pPrevSamplerHeap_)
			{
				pPrevSamplerHeap_ = pCurrentSamplerHeap_;
				changeHeap_ = true;
			}
		}

		if (changeHeap_)
		{
			ID3D12DescriptorHeap *pDescHeaps[2];
			int heap_cnt = 0;
			if (pDevice_->GetGlobalViewDescriptorHeap().GetHeap())
				pDescHeaps[heap_cnt++] = pDevice_->GetGlobalViewDescriptorHeap().GetHeap();
			if (pCurrentSamplerHeap_)
				pDescHeaps[heap_cnt++] = pCurrentSamplerHeap_.Get();
			pCmdList->SetDescriptorHeaps(heap_cnt, pDescHeaps);
			changeHeap_ = false;
		}

		if (sampler_count > 0)
		{
			// サンプラーステートのハンドルを登録する
			auto sampler_desc_size = pDevice_->GetDeviceDep()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			auto SetSamplerDesc = [&](u32 count, u8 index)
				{
					if (count > 0)
					{
						pCmdList->SetComputeRootDescriptorTable(index, samplerGpuHandle);
						samplerGpuHandle.ptr += sampler_desc_size * count;
					}
				};
			SetSamplerDesc(pDSet->GetCsSampler().maxCount, input_index.csSamplerIndex_);
		}

		// CBV, SRV, UAVの登録
		auto SetViewDesc = [&](u32 count, const D3D12_CPU_DESCRIPTOR_HANDLE *handles, u8 index)
			{
				if (count > 0)
				{
					std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> tmp;
					tmp.resize(count);
					for (u32 i = 0; i < count; i++)
					{
						tmp[i] = (handles[i].ptr > 0) ? handles[i] : def_view;
					}

					D3D12_CPU_DESCRIPTOR_HANDLE dst_cpu;
					D3D12_GPU_DESCRIPTOR_HANDLE dst_gpu;
					pViewDescStack_->Allocate(count, dst_cpu, dst_gpu);
					pDevice_->GetDeviceDep()->CopyDescriptors(
						1, &dst_cpu, &count,
						count, tmp.data(), nullptr,
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					pCmdList->SetComputeRootDescriptorTable(index, dst_gpu);
					tmp.clear();
				}
			};
		SetViewDesc(pDSet->GetCsCbv().maxCount, pDSet->GetCsCbv().cpuHandles, input_index.csCbvIndex_);
		SetViewDesc(pDSet->GetCsSrv().maxCount, pDSet->GetCsSrv().cpuHandles, input_index.csSrvIndex_);
		SetViewDesc(pDSet->GetCsUav().maxCount, pDSet->GetCsUav().cpuHandles, input_index.csUavIndex_);

		// set bindless srv.
		auto bindless_infos = pRS->GetBindlessInfos();
		if (ppBindlessArrays && bindless_infos.size() != 0)
		{
			auto bindless_array_count = bindless_infos.size();
			for (size_t ba = 0; ba < bindless_array_count; ba++)
			{
				if (ppBindlessArrays[ba])
				{
					auto &&bindless = *ppBindlessArrays[ba];
					SetViewDesc((u32)bindless.size(), bindless.data(), bindless_infos[ba].index_);
				}
			}
		}
	}


	void CommandList::SetDescriptorSet(DescriptorSet *pDSet)
	{
		auto pCmdList = GetCommandList();


	}

	bool CommandList::Init(Device *pDev, CommandQueue *pQueue)
    {
		//pDeviceに代入
		pDevice_ = pDev;
		if (!pDevice_)
		{
			return false;
		}
		//pQueueに代入
		pQueue_ = pQueue;
		if (!pQueue_)
		{
			return false;
		}
		//コマンドアロケータを生成
		
		auto hr = pDevice_->GetDeviceDep()->CreateCommandAllocator(pQueue_->listType_, IID_PPV_ARGS(&pAllocator_));
		if (FAILED(hr))
		{
			return false;
		}
		//コマンドリストを生成
		hr = pDevice_->GetDeviceDep()->CreateCommandList(0, pQueue_->listType_, pAllocator_.Get(), nullptr, IID_PPV_ARGS(&pCmdList_));
		if (FAILED(hr))
		{
			return false;
		}
		isReset_ = false;
		//コマンドリストを閉じる
		hr = pCmdList_->Close();
		assert(SUCCEEDED(hr));
		Reset();

		if (pQueue->listType_ == D3D12_COMMAND_LIST_TYPE_DIRECT || pQueue->listType_ == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			pViewDescStack_ = new DescriptorStackList();
			if (!pViewDescStack_->Initialilze(&pDev->GetGlobalViewDescriptorHeap()))
			{
				return false;
			}

			pSamplerDescCache_ = new SamplerDescriptorCache();
			if (!pSamplerDescCache_->Initialize(pDev))
			{
				return false;
			}

			pCurrentSamplerHeap_ = pPrevSamplerHeap_ = nullptr;
		}
		changeHeap_ = true;

		ym::ConsoleLog("コマンドリスト作成成功\n");
            return true;
    }

	void CommandList::Uninit()
	{
		pDevice_ = nullptr;
		pQueue_ = nullptr;


		pAllocator_.Reset();
		pCmdList_.Reset();

		if (pViewDescStack_)
		{
			pViewDescStack_->Destroy();
			delete pViewDescStack_;
			pViewDescStack_ = nullptr;
		}

		if (pSamplerDescCache_)
		{
			pSamplerDescCache_->Destroy();
			delete pSamplerDescCache_;
			pSamplerDescCache_ = nullptr;
		}

	}

	void CommandList::Reset()
	{
		if (isReset_)
		{
			return;
		}

		auto hr = pAllocator_->Reset();
		assert(SUCCEEDED(hr));

		hr = pCmdList_->Reset(pAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));

		if (pViewDescStack_)
			pViewDescStack_->Reset();

		isReset_ = true;
		changeHeap_ = true;
	}

	void CommandList::Close()
	{

		auto hr = pCmdList_->Close();
		assert(SUCCEEDED(hr));
	}
	void CommandList::Execute()
	{
		ID3D12CommandList *lists[] = { pCmdList_.Get() };
		pQueue_->GetQueueDep()->ExecuteCommandLists(_countof(lists), lists);
		isReset_ = false;
	}

}

