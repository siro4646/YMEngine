#pragma once

namespace ym
{
	class Device;
	class CommandList;
	class SwapChain;

	// テクスチャの次元
	struct TextureDimension
	{
		enum Type
		{
			Texture1D = 0,
			Texture2D,
			Texture3D,

			Max
		};
	};	// struct TextureDimension

	struct TextureDesc
	{
		TextureDimension::Type	dimension = TextureDimension::Texture2D;
		u32						width = 1, height = 1, depth = 1;
		u32						mipLevels = 1;
		DXGI_FORMAT				format = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_STATES	initialState = D3D12_RESOURCE_STATE_COMMON;
		u32						sampleCount = 1;
		float					clearColor[4] = { 0.0f };
		float					clearDepth = 1.0f;
		u8						clearStencil = 0;
		bool					isRenderTarget = false;
		bool					isDepthBuffer = false;
		bool					isUav = false;
	};

	class Texture
	{
		friend class CommandList;

	public:
		Texture(){}
		~Texture(){
		
		
				//pResource_->Release();
		}

		bool Init(Device *pDev, const TextureDesc &desc);
		bool InitFromSwapChain(Device*pDev,SwapChain *pSwapChain, u32 bufferIndex);

		//Getter
		ID3D12Resource *GetResourceDep() { return pResource_.Get(); }
		const TextureDesc &GetTextureDesc() const { return textureDesc_; }
		const D3D12_RESOURCE_DESC &GetResourceDesc() const { return resourceDesc_; }
	private:
		ComPtr<ID3D12Resource> pResource_{ nullptr };
		TextureDesc textureDesc_{};
		D3D12_RESOURCE_DESC resourceDesc_{};
		D3D12_RESOURCE_STATES	currentState_{ D3D12_RESOURCE_STATE_COMMON };
		D3D12_CLEAR_VALUE		clearValue_{};

	};
}
