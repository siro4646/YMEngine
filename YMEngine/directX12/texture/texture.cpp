#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"
#include "texture.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"
#include "swapChain/swapChain.h"

namespace ym
{


	bool Texture::Init(Device *pDev, const TextureDesc &desc)
	{
		const D3D12_RESOURCE_DIMENSION kDimensionTable[] = {
	D3D12_RESOURCE_DIMENSION_TEXTURE1D,
	D3D12_RESOURCE_DIMENSION_TEXTURE2D,
	D3D12_RESOURCE_DIMENSION_TEXTURE3D,
		};

		D3D12_HEAP_PROPERTIES prop{};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

		resourceDesc_.Dimension = kDimensionTable[desc.dimension];
		resourceDesc_.Alignment = 0;
		resourceDesc_.Width = desc.width;
		resourceDesc_.Height = desc.height;
		resourceDesc_.DepthOrArraySize = desc.depth;
		resourceDesc_.MipLevels = desc.mipLevels;
		resourceDesc_.Format = desc.format;
		resourceDesc_.SampleDesc.Count = desc.sampleCount;
		resourceDesc_.SampleDesc.Quality = 0;
		resourceDesc_.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc_.Flags = desc.isRenderTarget ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE;
		resourceDesc_.Flags |= desc.isDepthBuffer ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;
		resourceDesc_.Flags |= desc.isUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

		// 深度バッファの場合はリソースフォーマットをTYPELESSにしなければならない
		switch (resourceDesc_.Format)
		{
		case DXGI_FORMAT_D32_FLOAT:
			resourceDesc_.Format = DXGI_FORMAT_R32_TYPELESS; break;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			resourceDesc_.Format = DXGI_FORMAT_R32G8X24_TYPELESS; break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			resourceDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS; break;
		case DXGI_FORMAT_D16_UNORM:
			resourceDesc_.Format = DXGI_FORMAT_R16_TYPELESS; break;
		}

		currentState_ = desc.initialState;

		D3D12_CLEAR_VALUE *pClearValue = nullptr;
		D3D12_CLEAR_VALUE clearValue{};
		if (desc.isRenderTarget)
		{
			pClearValue = &clearValue_;
			clearValue_.Format = desc.format;
			memcpy(clearValue_.Color, desc.clearColor, sizeof(clearValue_.Color));

			if (currentState_ == D3D12_RESOURCE_STATE_COMMON)
				currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		else if (desc.isDepthBuffer)
		{
			pClearValue = &clearValue_;
			clearValue_.Format = desc.format;
			clearValue_.DepthStencil.Depth = desc.clearDepth;
			clearValue_.DepthStencil.Stencil = desc.clearStencil;

			if (currentState_ == D3D12_RESOURCE_STATE_COMMON)
				currentState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
		else
		{
			if (currentState_ == D3D12_RESOURCE_STATE_COMMON)
				currentState_ = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		auto hr = pDev->GetDeviceDep()->CreateCommittedResource(&prop, flags, &resourceDesc_, currentState_, pClearValue, IID_PPV_ARGS(&pResource_));
		if (FAILED(hr))
		{
			return false;
		}

		textureDesc_ = desc;

		return true;

	}

	bool Texture::InitFromSwapChain(Device *pDev, SwapChain *pSwapChain, u32 bufferIndex)
	{
		if (!pDev)
		{
			return false;
		}
		if (!pSwapChain)
		{
			return false;
		}

		auto hr = pSwapChain->GetSwapChainDep()->GetBuffer(bufferIndex, IID_PPV_ARGS(&pResource_));
		if (FAILED(hr))
		{
			return false;
		}
		resourceDesc_ = pResource_->GetDesc();

		memset(&textureDesc_, 0, sizeof(textureDesc_));
		textureDesc_.dimension = TextureDimension::Texture2D;
		textureDesc_.width = static_cast<u32>(resourceDesc_.Width);
		textureDesc_.height = resourceDesc_.Height;
		textureDesc_.depth = resourceDesc_.DepthOrArraySize;
		textureDesc_.mipLevels = resourceDesc_.MipLevels;
		textureDesc_.format = resourceDesc_.Format;
		textureDesc_.sampleCount = resourceDesc_.SampleDesc.Count;
		textureDesc_.isRenderTarget = true;

		currentState_ = D3D12_RESOURCE_STATE_PRESENT;

		return true;
	}

}