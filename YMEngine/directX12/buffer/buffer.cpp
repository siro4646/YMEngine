#include "buffer.h"
#include "../device/device.h"
#include "../commandList/commandList.h"
//#include "../fence/fence.h"
#include "resource/resourceStateTracker.h"

namespace ym
{
	//----
	bool Buffer::Init(Device *pDev, size_t size, size_t stride, BufferUsage::Type type, bool isDynamic, bool isUAV)
	{
		auto initialState = D3D12_RESOURCE_STATE_COMMON;
		//if (!isDynamic)
		//{
		//	initialState = D3D12_RESOURCE_STATE_COPY_DEST;
		//}
		if (type == BufferUsage::ReadBack)
		{
			initialState = D3D12_RESOURCE_STATE_COPY_DEST;
		}
		if (type == BufferUsage::ShaderResource)
		{
			//initialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			//initialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		}
		auto rST = ResourceStateTracker::Instance();

		// ���\�[�X�̏�����
		rST->SetState(pResource_, initialState);

		return Init(pDev, size, stride, type, initialState, isDynamic, isUAV);
	}

	//----
	bool Buffer::Init(Device *pDev, size_t size, size_t stride, BufferUsage::Type type, D3D12_RESOURCE_STATES initialState, bool isDynamic, bool isUAV)
	{
		D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD;
		if (!isDynamic)
		{
			heapType = D3D12_HEAP_TYPE_DEFAULT;
		}
		if (type == BufferUsage::ReadBack)
		{
			heapType = D3D12_HEAP_TYPE_READBACK;
		}

		size_t allocSize = size;	
		if (type == BufferUsage::ConstantBuffer)
		{
			allocSize = GetAlignedSize(allocSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			//ym::ConsoleLogRelease("���o�b�t�@�̃T�C�Y : %d, �A���C�����g��̃T�C�Y : %d\n", size, allocSize);
		}

		// ByteAddressBuffer�̏ꍇ��R32_TYPELESS�ɐݒ肷��
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		D3D12_HEAP_PROPERTIES prop{};
		prop.Type = heapType;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = allocSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = isUAV ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

		currentState_ = initialState;

		auto hr = pDev->GetDeviceDep()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, currentState_, nullptr, IID_PPV_ARGS(&pResource_));
		if (FAILED(hr))
		{
			return false;
		}

		resourceDesc_ = desc;
		heapProp_ = prop;
		size_ = size;
		stride_ = stride;
		bufferUsage_ = type;
		isUAV_ = isUAV;
		return true;
	}

	//----
	void Buffer::Destroy()
	{
		SafeRelease(pResource_);
	}

	//----
	void Buffer::UpdateBuffer(Device *pDev, CommandList *pCmdList, const void *pData, size_t size, size_t offset)
	{
		if (!pDev || !pCmdList)
		{
			return;
		}
		if (!pData || !size)
		{
			return;
		}
		if (offset + size > size_)
		{
			return;
		}

		if (heapProp_.Type == D3D12_HEAP_TYPE_UPLOAD)
		{
			u8 *p = reinterpret_cast<u8 *>(Map());
			if (p) {
				memcpy(p + offset, pData, size);
				//std::cout << "Data copied to upload buffer" << std::endl;
			}
				Unmap();
		}
		else
		{
			Buffer *src = new Buffer();
			if (!src->Init(pDev, size, stride_, bufferUsage_, true, false))
			{
				return;
			}
			src->UpdateBuffer(pDev, pCmdList, pData, size, 0);			

			pCmdList->GetCommandList()->CopyBufferRegion(pResource_, offset, src->pResource_, 0, size);
			auto rST = ResourceStateTracker::Instance();
			if (bufferUsage_ == BufferUsage::IndexBuffer)
			{
				pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_INDEX_BUFFER);
			}
			else if (bufferUsage_ == BufferUsage::VertexBuffer)
			{
				pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			}
			/*else if (bufferUsage_ == BufferUsage::ConstantBuffer)
			{
				pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			}
			else if (bufferUsage_ == BufferUsage::ReadBack)
			{
				pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_DEST);
			}*/
			else if (bufferUsage_ == BufferUsage::ShaderResource)
			{

				//pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			}
/*			else
			{
				pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
			}*/	
			//pCmdList->TransitionBarrier(this,)


			pDev->KillObject(src);
		}

	}

	//----
	void *Buffer::Map(CommandList *)
	{
		return Map();
	}
	void *Buffer::Map()
	{
		if (!pResource_)
		{
			return nullptr;
		}

		void *pData{ nullptr };
		auto hr = pResource_->Map(0, nullptr, &pData);
		if (FAILED(hr))
		{
			return nullptr;
		}
		return pData;
	}
	void *Buffer::Map(const D3D12_RANGE &range)
	{
		if (!pResource_)
		{
			return nullptr;
		}

		void *pData{ nullptr };
		auto hr = pResource_->Map(0, &range, &pData);
		if (FAILED(hr))
		{
			return nullptr;
		}
		return (u8 *)pData + range.Begin;
	}

	//----
	void Buffer::Unmap()
	{
		if (!pResource_)
		{
			return;
		}

		pResource_->Unmap(0, nullptr);
	}
	void Buffer::Unmap(const D3D12_RANGE &range)
	{
		if (!pResource_)
		{
			return;
		}

		pResource_->Unmap(0, &range);
	}

}	// namespace ym

//	EOF
