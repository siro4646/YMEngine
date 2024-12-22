#include "commandList.h"
#include "device/device.h"
#include "commandQueue/commandQueue.h"
#include "texture/texture.h"

namespace ym
{
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
		}
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
		ym::ConsoleLog("コマンドリスト作成成功\n");
            return true;
    }

	void CommandList::Uninit()
	{
		pAllocator_.Reset();
		pCmdList_.Reset();
		pDevice_ = nullptr;
		pQueue_ = nullptr;
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
		isReset_ = true;
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

