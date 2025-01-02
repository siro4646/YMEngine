#pragma once

#include "resource/resourceStateTracker.h"


namespace ym
{
	class Device;
	class CommandQueue;
	class Texture;
	class Buffer;
	class DescriptorStackList;
	class SamplerDescriptorCache;
	class RootSignature;
	class DescriptorSet;

	class CommandList : public std::enable_shared_from_this<CommandList>
	{
	public:
		CommandList()
		{}
		~CommandList()
		{
			Uninit();
		}
		void TransitionBarrier(Texture *p,D3D12_RESOURCE_STATES nextState);
		void TransitionBarrier(Texture *p, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES nextState);
		void TransitionBarrier(Buffer *p, D3D12_RESOURCE_STATES nextState);
		void TransitionBarrier(Buffer *p, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES nextState);

		// RootSignatureとDescriptorをコマンドリストに積み込む
		void SetGraphicsRootSignatureAndDescriptorSet(RootSignature *pRS, DescriptorSet *pDSet, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> **ppBindlessArrays = nullptr);
		// Descriptorだけをコマンドリストに積み込む
		void SetDescriptorSet(DescriptorSet *pDSet);

		void SetDescriptorHeapDirty()
		{
			changeHeap_ = true;
		}

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

		DescriptorStackList *pViewDescStack_{ nullptr };
		SamplerDescriptorCache *pSamplerDescCache_{ nullptr };

		ComPtr<ID3D12DescriptorHeap>pCurrentSamplerHeap_{ nullptr };
		ComPtr<ID3D12DescriptorHeap>pPrevSamplerHeap_{ nullptr };
		//リセットしたかどうか
		bool isReset_{ false };

		//ディスクリプタヒープを変更するかどうか
		bool						changeHeap_{ true };



	};

}