#pragma once

namespace ym
{
	class Device;
	class CommandQueue;
	class Texture;

	class CommandList
	{
	public:
		CommandList()
		{}
		~CommandList()
		{
			Uninit();
		}
		void TransitionBarrier(Texture *p, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES nextState);


		bool Init(Device *pDev, CommandQueue *pQueue);
		void Uninit();

		void Reset();
		void Close();
		void Execute();
		//Getter
		ID3D12GraphicsCommandList *GetCommandList() const { return pCmdList_.Get(); }

	private:
		Device* pDevice_{};
		CommandQueue *pQueue_{};
		ComPtr<ID3D12CommandAllocator> pAllocator_{ nullptr };
		ComPtr<ID3D12GraphicsCommandList> pCmdList_{ nullptr };

		//ƒŠƒZƒbƒg‚µ‚½‚©‚Ç‚¤‚©
		bool isReset_{ false };



	};

}