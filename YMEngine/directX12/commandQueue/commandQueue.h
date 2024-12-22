#pragma once

namespace ym
{
	class Device;

	class CommandQueue
	{
		friend class CommandList;

	public:
		CommandQueue()
		{}
		~CommandQueue()
		{
			Uninit();
		}

		bool Init(Device *pDev, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT, u32 priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL);
		void Uninit();


		ID3D12CommandQueue *GetQueueDep() { return pQueue_.Get(); }

	private:
		ComPtr<ID3D12CommandQueue>pQueue_{ nullptr };
		D3D12_COMMAND_LIST_TYPE listType_{ D3D12_COMMAND_LIST_TYPE_DIRECT };
	};	// class CommandQueue

}	// namespace ym