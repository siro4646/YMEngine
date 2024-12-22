#include "commandQueue.h"
#include "device/device.h"

namespace ym
{
	//----
	bool CommandQueue::Init(Device *pDev, D3D12_COMMAND_LIST_TYPE type, u32 priority)
	{
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Type = type;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		// GPUタイムアウトが有効
		desc.Priority = priority;
		auto hr = pDev->GetDeviceDep()->CreateCommandQueue(&desc, IID_PPV_ARGS(&pQueue_));
		if (FAILED(hr))
		{
			return false;
		}

		listType_ = type;
		return true;
	}
	//----
	void CommandQueue::Uninit()
	{
		//SafeRelease(pQueue_);
	}
}

