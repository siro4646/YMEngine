#include "texture.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"
#include "swapChain/swapChain.h"

#include "resource/resourceStateTracker.h"

#include "utility/fileStream/fileStream.h"

#include "resource/resourceManager.h"

#include "Renderer/renderer.h"

namespace ym
{
	std::unordered_map<string, ym::Texture> Texture::textureMap_{};

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
		if (desc.isDepthBuffer)
		{
			ym::ConsoleLog("depth buffer\n");
		}
			//resourceDesc_.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

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
		auto rST = ResourceStateTracker::Instance();
		auto hr = pDev->GetDeviceDep()->CreateCommittedResource(&prop, flags, &resourceDesc_, currentState_, pClearValue, IID_PPV_ARGS(&pResource_));
		rST->SetState(pResource_.Get(), currentState_);
		if (FAILED(hr))
		{
			return false;
		}

		textureDesc_ = desc;

		return true;

	}

	bool Texture::LoadTexture(Device *pDev, CommandList *pCmdList, const std::string filename, bool isForceSRGB, bool forceSysRam)
	{
		textureName_ = filename;
		auto rM = ResourceManager::Instance();
		//auto textureMap = rM->GetTextureMap();
		if (rM->FindTexture(filename))
		{
			*this = rM->GetTexture(filename);
			return true;
		}	
		// ファイルの読み込み
		ym::File file(filename.c_str());

		bool res = false;
		//拡張子で分岐
		if (ym::GetExtent(filename.c_str()) == ".tga")
		{
			res = InitFromTGA(pDev, pCmdList, file.GetData(), file.GetSize(), 1, isForceSRGB, forceSysRam);
			//return res;
		}
		else if (ym::GetExtent(filename.c_str()) == ".dds")
		{
			res = InitFromDDS(pDev, pCmdList, file.GetData(), file.GetSize(), 1, isForceSRGB, forceSysRam);

		}
		else if (ym::GetExtent(filename.c_str()) == ".png")
		{
			res = InitFromPNG(pDev, pCmdList, file.GetData(), file.GetSize(), 1, isForceSRGB, forceSysRam);

		}
		else if (ym::GetExtent(filename.c_str()) == ".jpg")
		{
			res = InitFromJPG(pDev, pCmdList, file.GetData(), file.GetSize(), 1, isForceSRGB, forceSysRam);

		}
		//psd拡張子はまだ対応していないのでtgaに変換
		else if (ym::GetExtent(filename.c_str()) == ".psd")
		{
			auto wfilename = ym::Utf8ToUtf16(filename.c_str());

			std::wstring wpath = ym::ReplaceExtension(wfilename.c_str(), ".tga");

			auto path = ym::Utf16ToUtf8(wpath.c_str());

			res = LoadTexture(pDev, pCmdList, path, isForceSRGB, forceSysRam);


			//res = LoadTexture(pDev, pCmdList, path, isForceSRGB, forceSysRam);

		}
		//拡張子がない場合
		else
		{

		}
		if (res)
		{
			//textureMap_.get()->emplace(std::make_pair(filename, *this));
			rM->SetTexture(filename, *this);
			//textureMap->insert({ filename,*this});
			//textureMap[filename] = *this;
			//バリアを張る
			auto rST = ResourceStateTracker::Instance();
			pCmdList->TransitionBarrier(this, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			pCmdList->Close();
			pCmdList->Execute();
			pDev->WaitForGraphicsCommandQueue();
			pCmdList->Reset();
		}
		else
		{
			ym::DebugLog("failed to load texture");
			ym::DebugLog("[%s]\n", filename.c_str());
			//LoadTexture(pDev, pCmdList, "asset/texture/test.png", isForceSRGB, forceSysRam);
			static auto renderer = Renderer::Instance();
			*this = *renderer->GetDummyTexture(DummyTex::White);
		}
		return res;


	}

	bool Texture::LoadTexture(Device *pDev, CommandList *pCmdList, const std::wstring *filename, bool isForceSRGB, bool forceSysRam)
	{
		//wstring -> string
		std::string strFilename(filename->begin(), filename->end());

		return LoadTexture(pDev, pCmdList, strFilename, isForceSRGB, forceSysRam);

	}

	bool Texture::InitFromDXImage(Device *pDev, const DirectX::ScratchImage &image, bool isForceSRGB, bool forceSysRam)
	{
		if (!pDev)
		{
			return false;
		}

		// テクスチャオブジェクト作成
		const DirectX::TexMetadata &meta = image.GetMetadata();
		if (!meta.mipLevels || !meta.arraySize)
		{
			return false;
		}
		if ((meta.width > UINT32_MAX) || (meta.height > UINT32_MAX)
			|| (meta.mipLevels > UINT16_MAX) || (meta.arraySize > UINT16_MAX))
		{
			return false;
		}

		DXGI_FORMAT format = meta.format;
		if (isForceSRGB)
		{
			switch (format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM: format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC1_UNORM: format = DXGI_FORMAT_BC1_UNORM_SRGB; break;
			case DXGI_FORMAT_BC2_UNORM: format = DXGI_FORMAT_BC2_UNORM_SRGB; break;
			case DXGI_FORMAT_BC3_UNORM: format = DXGI_FORMAT_BC3_UNORM_SRGB; break;
			case DXGI_FORMAT_B8G8R8A8_UNORM: format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_B8G8R8X8_UNORM: format = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC7_UNORM: format = DXGI_FORMAT_BC7_UNORM_SRGB; break;
			}
		}

		D3D12_RESOURCE_DESC desc = {};
		desc.Width = static_cast<u32>(meta.width);
		desc.Height = static_cast<u32>(meta.height);
		desc.MipLevels = static_cast<u16>(meta.mipLevels);
		desc.DepthOrArraySize = (meta.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
			? static_cast<u16>(meta.depth)
			: static_cast<u16>(meta.arraySize);
		desc.Format = format;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.SampleDesc.Count = 1;
		desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(meta.dimension);

		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		if (forceSysRam)
		{
			heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		}

		auto hr = pDev->GetDeviceDep()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&pResource_));
		if (FAILED(hr))
		{
			return false;
		}

		// 情報を格納
		resourceDesc_ = desc;
		ResourceStateTracker::Instance()->SetState(pResource_.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
		currentState_ = ResourceStateTracker::Instance()->GetState(pResource_.Get());
		memset(&textureDesc_, 0, sizeof(textureDesc_));
		textureDesc_.width = static_cast<u32>(desc.Width);
		textureDesc_.height = desc.Height;
		textureDesc_.depth = desc.DepthOrArraySize;
		textureDesc_.mipLevels = desc.MipLevels;
		textureDesc_.sampleCount = 1;
		textureDesc_.format = desc.Format;
		switch (meta.dimension)
		{
		case DirectX::TEX_DIMENSION_TEXTURE1D: textureDesc_.dimension = TextureDimension::Texture1D; break;
		case DirectX::TEX_DIMENSION_TEXTURE2D: textureDesc_.dimension = TextureDimension::Texture2D; break;
		case DirectX::TEX_DIMENSION_TEXTURE3D: textureDesc_.dimension = TextureDimension::Texture3D; break;
		default: textureDesc_.dimension = TextureDimension::Max;
		}

		return true;
	}


	bool Texture::InitFromTGA(Device *pDev, CommandList *pCmdList, const void *pTgaBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam)
	{
		auto image = InitializeFromTGAwoLoad(pDev, pTgaBin, size, mipLevels);
		if (image.get() == nullptr)
		{
			return false;
		}

		// D3D12リソースを作成
		if (!InitFromDXImage(pDev, *image, isForceSRGB, forceSysRam))
		{
			return false;
		}

		// コピー命令発行
		ID3D12Resource *pSrcImage = nullptr;
		if (!UpdateImage(pDev, pCmdList, *image, &pSrcImage))
		{
			return false;
		}
		pDev->PendingKill(new ReleaseObjectItem<ID3D12Resource>(pSrcImage));

		return true;
	}
	std::unique_ptr<DirectX::ScratchImage> Texture::InitializeFromTGAwoLoad(Device *pDev, const void *pTgaBin, size_t size, ym::u32 mipLevels)
	{
		if (!pDev)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}
		if (!pTgaBin || !size)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}

		// TGAファイルフォーマットからイメージリソースを作成
		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage());
		//DirectX::LoadFromTGAMemory()

		auto hr = DirectX::LoadFromTGAMemory((const u8 *)pTgaBin, size, nullptr, *image);
		if (FAILED(hr))
		{
			return nullptr;
		}

		// ミップマップ生成
		if (mipLevels != 1)
		{
			std::unique_ptr<DirectX::ScratchImage> mipped_image(new DirectX::ScratchImage());
			DirectX::GenerateMipMaps(*image->GetImage(0, 0, 0), DirectX::TEX_FILTER_CUBIC | DirectX::TEX_FILTER_FORCE_NON_WIC, 0, *mipped_image);
			image.swap(mipped_image);
		}

		return std::move(image);
	}

	bool Texture::InitFromDDS(Device *pDev, CommandList *pCmdList, const void *pDdsBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam)
	{
		auto image = InitializeFromDDSwoLoad(pDev, pDdsBin, size, mipLevels);
		if (image.get() == nullptr)
		{
			return false;
		}

		// D3D12リソースを作成
		if (!InitFromDXImage(pDev, *image, isForceSRGB, forceSysRam))
		{
			return false;
		}

		// コピー命令発行
		ID3D12Resource *pSrcImage = nullptr;
		if (!UpdateImage(pDev, pCmdList, *image, &pSrcImage))
		{
			return false;
		}
		pDev->PendingKill(new ReleaseObjectItem<ID3D12Resource>(pSrcImage));

		return true;
	}

	std::unique_ptr<DirectX::ScratchImage> Texture::InitializeFromDDSwoLoad(Device *pDev, const void *pTgaBin, size_t size, ym::u32 mipLevels)
	{
		if (!pDev)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}
		if (!pTgaBin || !size)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}

		// TGAファイルフォーマットからイメージリソースを作成
		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage());
		DirectX::TexMetadata info{};
		auto hr = DirectX::LoadFromDDSMemory((const u8 *)pTgaBin, size, DirectX::DDS_FLAGS_NONE, &info, *image);
		if (FAILED(hr))
		{
			return nullptr;
		}

		// ミップマップ生成
		if (mipLevels != 1 && info.mipLevels == 1)
		{
			std::unique_ptr<DirectX::ScratchImage> mipped_image(new DirectX::ScratchImage());
			DirectX::GenerateMipMaps(*image->GetImage(0, 0, 0), DirectX::TEX_FILTER_CUBIC | DirectX::TEX_FILTER_FORCE_NON_WIC, 0, *mipped_image);
			image.swap(mipped_image);
		}


		return std::move(image);
	}

	//----
	bool Texture::InitFromPNG(Device *pDev, CommandList *pCmdList, const void *pPngBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam)
	{
		auto image = InitializeFromPNGwoLoad(pDev, pPngBin, size, mipLevels);
		if (image.get() == nullptr)
		{
			return false;
		}

		// D3D12リソースを作成
		if (!InitFromDXImage(pDev, *image, isForceSRGB, forceSysRam))
		{
			return false;
		}

		// コピー命令発行
		ID3D12Resource *pSrcImage = nullptr;
		if (!UpdateImage(pDev, pCmdList, *image, &pSrcImage))
		{
			return false;
		}
		pDev->PendingKill(new ReleaseObjectItem<ID3D12Resource>(pSrcImage));

		return true;
	}

	bool Texture::InitFromJPG(Device *pDev, CommandList *pCmdList, const void *pJpgBin, size_t size, ym::u32 mipLevels, bool isForceSRGB, bool forceSysRam)
	{
		auto image = InitializeFromJPGwoLoad(pDev, pJpgBin, size, mipLevels);
		if (image.get() == nullptr)
		{
			return false;
		}

		// D3D12リソースを作成
		if (!InitFromDXImage(pDev, *image, isForceSRGB, forceSysRam))
		{
			return false;
		}

		// コピー命令発行
		ID3D12Resource *pSrcImage = nullptr;
		if (!UpdateImage(pDev, pCmdList, *image, &pSrcImage))
		{
			return false;
		}
		pDev->PendingKill(new ReleaseObjectItem<ID3D12Resource>(pSrcImage));

		return true;
	}

	//----
	std::unique_ptr<DirectX::ScratchImage> Texture::InitializeFromPNGwoLoad(Device *pDev, const void *pPngBin, size_t size, ym::u32 mipLevels)
	{
		if (!pDev)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}
		if (!pPngBin || !size)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}

		// DirectXTexを利用してPNGデータをメモリからロード
		DirectX::TexMetadata metadata;
		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage());
		HRESULT hr = DirectX::LoadFromWICMemory(
			(const u8 *)pPngBin,                       // PNGデータ
			size,                          // PNGデータのサイズ
			DirectX::WIC_FLAGS_NONE,       // WICのフラグ
			&metadata,                     // メタデータの取得
			*image                         // 読み込んだテクスチャデータ
		);

		if (FAILED(hr))
		{
			return nullptr; // ロード失敗時
		}

		// RGBA フォーマットへの変換（必要な場合）
		if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			std::unique_ptr<DirectX::ScratchImage> convertedImage(new DirectX::ScratchImage());
			hr = DirectX::Convert(
				*image->GetImage(0, 0, 0),            // 元のイメージ
				DXGI_FORMAT_R8G8B8A8_UNORM,           // 変換先フォーマット
				DirectX::TEX_FILTER_DEFAULT,          // フィルタリング設定
				DirectX::TEX_THRESHOLD_DEFAULT,       // スレッショルド（デフォルト）
				*convertedImage                       // 変換後のイメージ
			);

			if (FAILED(hr))
			{
				return nullptr; // 変換失敗時
			}

			image.swap(convertedImage); // 変換後のイメージを入れ替え
		}

		// ミップマップ生成（必要な場合）
		if (mipLevels != 1)
		{
			std::unique_ptr<DirectX::ScratchImage> mippedImage(new DirectX::ScratchImage());
			hr = DirectX::GenerateMipMaps(
				*image->GetImage(0, 0, 0),             // 元のイメージ
				DirectX::TEX_FILTER_CUBIC | DirectX::TEX_FILTER_FORCE_NON_WIC, // フィルタリング設定
				mipLevels,                             // ミップレベル
				*mippedImage                           // 出力先
			);

			if (FAILED(hr))
			{
				return nullptr; // ミップマップ生成失敗時
			}

			image.swap(mippedImage); // ミップマップ生成後のイメージを入れ替え
		}

		return std::move(image);
	}

	std::unique_ptr<DirectX::ScratchImage> Texture::InitializeFromJPGwoLoad(Device *pDev, const void *pJpgBin, size_t size, ym::u32 mipLevels)
	{
		if (!pDev)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}
		if (!pJpgBin || !size)
		{
			return std::unique_ptr<DirectX::ScratchImage>();
		}	
		// DirectXTexを利用してJPGデータをメモリからロード
		DirectX::TexMetadata metadata;
		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage());
		HRESULT hr = DirectX::LoadFromWICMemory(
			(const u8 *)pJpgBin,                       // JPGデータ
			size,                          // JPGデータのサイズ
			DirectX::WIC_FLAGS_NONE,       // WICのフラグ
			&metadata,                     // メタデータの取得
			*image                         // 読み込んだテクスチャデータ
		);

		if (FAILED(hr))
		{
			return nullptr; // ロード失敗時
		}

		// RGBA フォーマットへの変換（必要な場合）
		if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			std::unique_ptr<DirectX::ScratchImage> convertedImage(new DirectX::ScratchImage());
			hr = DirectX::Convert(
				*image->GetImage(0, 0, 0),            // 元のイメージ
				DXGI_FORMAT_R8G8B8A8_UNORM,           // 変換先フォーマット
				DirectX::TEX_FILTER_DEFAULT,          // フィルタリング設定
				DirectX::TEX_THRESHOLD_DEFAULT,       // スレッショルド（デフォルト）
				*convertedImage                       // 変換後のイメージ
			);

			if (FAILED(hr))
			{
				return nullptr; // 変換失敗時
			}

			image.swap(convertedImage); // 変換後のイメージを入れ替え
		}

		// ミップマップ生成（必要な場合）
		if (mipLevels != 1)
		{
			std::unique_ptr<DirectX::ScratchImage> mippedImage(new DirectX::ScratchImage());
			hr = DirectX::GenerateMipMaps(
				*image->GetImage(0, 0, 0),             // 元のイメージ
				DirectX::TEX_FILTER_CUBIC | DirectX::TEX_FILTER_FORCE_NON_WIC, // フィルタリング設定
				mipLevels,                             // ミップレベル
				*mippedImage                           // 出力先
			);

			if (FAILED(hr))
			{
				return nullptr; // ミップマップ生成失敗時
			}

			image.swap(mippedImage); // ミップマップ生成後のイメージを入れ替え
		}

		return std::move(image);
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

	bool Texture::InitFromBin(Device *pDev, CommandList *pCmdList, const TextureDesc &desc, const void *pImageBin)
	{
		if (!pDev)
		{
			return false;
		}
		if (!pImageBin)
		{
			return false;
		}

		// テクスチャオブジェクト作成
		if (!Init(pDev, desc))
		{
			return false;
		}

		// リソースにデータをコピー
		ID3D12Resource *pSrcImage = nullptr;
		if (!UpdateImage(pDev, pCmdList, pImageBin, &pSrcImage))
		{
			return false;
		}
		pDev->PendingKill(new ReleaseObjectItem<ID3D12Resource>(pSrcImage));
		return true;

	}

	//----
	bool Texture::UpdateImage(Device *pDev, CommandList *pCmdList, const DirectX::ScratchImage &image, ID3D12Resource **ppSrcImage)
	{
		const DirectX::TexMetadata &meta = image.GetMetadata();

		// リソースのサイズ等を取得
		u32 numSubresources = static_cast<u32>(meta.arraySize * meta.mipLevels);
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprint(numSubresources);
		std::vector<u32> numRows(numSubresources);
		std::vector<u64> rowSize(numSubresources);
		u64 totalSize;
		pDev->GetDeviceDep()->GetCopyableFootprints(&resourceDesc_, 0, numSubresources, 0, footprint.data(), numRows.data(), rowSize.data(), &totalSize);

		// アップロード用のオブジェクトを作成
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = totalSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Resource *pSrcImage{ nullptr };
		auto hr = pDev->GetDeviceDep()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pSrcImage));
		if (FAILED(hr))
		{
			return false;
		}

		// リソースをマップしてイメージ情報をコピー
		u8 *pData{ nullptr };
		hr = pSrcImage->Map(0, NULL, reinterpret_cast<void **>(&pData));
		if (FAILED(hr))
		{
			SafeRelease(pSrcImage);
			return false;
		}

		for (u32 d = 0; d < meta.arraySize; d++)
		{
			for (u32 m = 0; m < meta.mipLevels; m++)
			{
				size_t i = d * meta.mipLevels + m;
				if (rowSize[i] > (SIZE_T) -1)
				{
					SafeRelease(pSrcImage);
					return false;
				}
				D3D12_MEMCPY_DEST dstData = { pData + footprint[i].Offset, footprint[i].Footprint.RowPitch, footprint[i].Footprint.RowPitch * numRows[i] };
				const DirectX::Image *pImage = image.GetImage(m, 0, d);
				if (rowSize[i] == footprint[i].Footprint.RowPitch)
				{
					memcpy(dstData.pData, pImage->pixels, pImage->rowPitch * numRows[i]);
				}
				else if (rowSize[i] < footprint[i].Footprint.RowPitch)
				{
					u8 *p_src = pImage->pixels;
					u8 *p_dst = reinterpret_cast<u8 *>(dstData.pData);
					for (u32 r = 0; r < numRows[i]; r++, p_src += pImage->rowPitch, p_dst += footprint[i].Footprint.RowPitch)
					{
						memcpy(p_dst, p_src, rowSize[i]);
					}
				}
			}
		}

		pSrcImage->Unmap(0, nullptr);

		// コピー命令を発行
		for (u32 i = 0; i < numSubresources; i++)
		{
			D3D12_TEXTURE_COPY_LOCATION src, dst;
			src.pResource = pSrcImage;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint = footprint[i];
			dst.pResource = pResource_.Get();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = i;
			pCmdList->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		}

		*ppSrcImage = pSrcImage;

		return true;
	}

	//----
	bool Texture::UpdateImage(Device *pDev, CommandList *pCmdList, const void *pImageBin, ID3D12Resource **ppSrcImage)
	{
		// リソースのサイズ等を取得
		u32 numSubresources = resourceDesc_.DepthOrArraySize * resourceDesc_.MipLevels;
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprint(numSubresources);
		std::vector<u32> numRows(numSubresources);
		std::vector<u64> rowSize(numSubresources);
		u64 totalSize;
		pDev->GetDeviceDep()->GetCopyableFootprints(&resourceDesc_, 0, numSubresources, 0, footprint.data(), numRows.data(), rowSize.data(), &totalSize);

		// アップロード用のオブジェクトを作成
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = totalSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Resource *pSrcImage{ nullptr };
		auto hr = pDev->GetDeviceDep()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pSrcImage));
		if (FAILED(hr))
		{
			return false;
		}

		// リソースをマップしてイメージ情報をコピー
		u8 *pData{ nullptr };
		hr = pSrcImage->Map(0, NULL, reinterpret_cast<void **>(&pData));
		if (FAILED(hr))
		{
			SafeRelease(pSrcImage);
			return false;
		}

		{
			if (rowSize[0] > (SIZE_T)-1)
			{
				SafeRelease(pSrcImage);
				return false;
			}
			D3D12_MEMCPY_DEST dstData = { pData + footprint[0].Offset, footprint[0].Footprint.RowPitch, footprint[0].Footprint.RowPitch * numRows[0] };
			memcpy(dstData.pData, pImageBin, footprint[0].Footprint.RowPitch * footprint[0].Footprint.Height);
		}

		pSrcImage->Unmap(0, nullptr);

		// コピー命令を発行
		for (u32 i = 0; i < numSubresources; i++)
		{
			D3D12_TEXTURE_COPY_LOCATION src, dst;
			src.pResource = pSrcImage;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint = footprint[i];
			dst.pResource = pResource_.Get();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = i;
			pCmdList->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		}

		*ppSrcImage = pSrcImage;

		return true;
	}


}