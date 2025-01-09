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

	enum TextureState
	{
		//初期化前
		Uninitialized = 0,
		//初期化済み
		Initialized,
		//読み込み終了
		Loaded,
	};

	class Texture
	{
		friend class CommandList;

	public:
		

		Texture(){}
		~Texture(){
			Uninit();
		}

		bool Init(Device *pDev, const TextureDesc &desc);

		void Uninit()
		{
			pResource_.Reset();
		}

		bool InitFromSwapChain(Device*pDev,SwapChain *pSwapChain, u32 bufferIndex);

		bool InitFromBin(Device *pDev, CommandList *pCmdList, const TextureDesc &desc, const void *pImageBin);

		bool LoadTexture(Device *pDev, CommandList *pCmdList, const std::string filename, bool isForceSRGB = false, bool forceSysRam = false);
		bool LoadTexture(Device *pDev, CommandList *pCmdList, const std::wstring *filename, bool isForceSRGB = false, bool forceSysRam = false);


		bool UpdateImage(Device *pDev, CommandList *pCmdList, const DirectX::ScratchImage &image, ID3D12Resource **ppSrcImage);
		bool UpdateImage(Device *pDev, CommandList *pCmdList, const void *pImageBin, ID3D12Resource **ppSrcImage);

		static void ResourceRelease()
		{
			textureMap_.clear();
		}

		//Getter
		ID3D12Resource *GetResourceDep() { return pResource_.Get(); }
		const TextureDesc &GetTextureDesc() const { return textureDesc_; }
		const D3D12_RESOURCE_DESC &GetResourceDesc() const { return resourceDesc_; }

	private:

		bool InitFromDXImage(Device *pDev, const DirectX::ScratchImage &image, bool isForceSRGB, bool forceSysRam = false);

		bool InitFromTGA(Device *pDev, CommandList *pCmdList, const void *pTgaBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam = false);
		bool InitFromDDS(Device *pDev, CommandList *pCmdList, const void *pDdsBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam = false);
		bool InitFromPNG(Device *pDev, CommandList *pCmdList, const void *pPngBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam = false);
		bool InitFromJPG(Device *pDev, CommandList *pCmdList, const void *pJpgBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam = false);





		std::unique_ptr<DirectX::ScratchImage> InitializeFromTGAwoLoad(Device *pDev, const void *pTgaBin, size_t size, ym::u32 mipLevels);
		std::unique_ptr<DirectX::ScratchImage> InitializeFromDDSwoLoad(Device *pDev, const void *pTgaBin, size_t size, ym::u32 mipLevels);
		std::unique_ptr<DirectX::ScratchImage> InitializeFromPNGwoLoad(Device *pDev, const void *pPngBin, size_t size, ym::u32 mipLevels);
		std::unique_ptr<DirectX::ScratchImage> InitializeFromJPGwoLoad(Device *pDev, const void *pJpgBin, size_t size, ym::u32 mipLevels);

	private:
		static  std::unordered_map<std::string,ym::Texture> textureMap_;

		ComPtr<ID3D12Resource> pResource_{ nullptr };
		TextureDesc textureDesc_{};
		D3D12_RESOURCE_DESC resourceDesc_{};
		D3D12_RESOURCE_STATES	currentState_{ D3D12_RESOURCE_STATE_COMMON };
		D3D12_CLEAR_VALUE		clearValue_{};

	};
}
