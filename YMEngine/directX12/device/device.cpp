#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"
#include "device.h"
#include "commandQueue/commandQueue.h"
#include "swapChain/swapChain.h"


namespace ym {
	Device::Device()
	{
	}
	Device::~Device()
	{
		pFactory_.Reset();
		pDevice_.Reset();
	}
	bool Device::Init(HWND hwnd, u32 width, u32 height, ColorSpaceType csType)
	{
		if (!CreateFactory())
		{
			return false;
		}
		if (!CreateDevice(csType))
		{
			return false;
		}
		if (!CreateCommandQueue())
		{
			return false;
		}

		if (!CreateDescriptorHeap())
		{
			return false;
		}
		if (!CreateSwapChain(hwnd, width, height))
		{
			return false;
		}
		if (!CreateFence())
		{
			return false;
		}

		return true;
	}
	void Device::Uninit()
	{
		WaitForCommandQueue();

	/*	pGlobalViewDescHeap_->Destroy();
		delete pGlobalViewDescHeap_;
		pGlobalViewDescHeap_ = nullptr;

		defaultSamplerDescInfo_.Free();
		defaultViewDescInfo_.Free();

		pSamplerDescHeap_->Destroy();
		delete pSamplerDescHeap_;
		pSamplerDescHeap_ = nullptr;

		pViewDescHeap_->Destroy();
		delete pViewDescHeap_;
		pViewDescHeap_ = nullptr;*/


		
		


	}
	void Device::WaitForCommandQueue()
	{
		pGraphicsQueue_->GetQueueDep()->Signal(pFence_.Get(), ++fenceValue_);
		if (pFence_->GetCompletedValue() != fenceValue_)
		{
			auto event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if(!event)
			{
				assert(0, &&"イベントエラー、アプリケーション終了\n");
			}
			pFence_->SetEventOnCompletion(fenceValue_, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}
	void Device::Present(int syncInterval)
	{
		pSwapChain_->Present(syncInterval);
	}
	void Device::Resize(u32 width, u32 height)
	{	
		pSwapChain_->Resize(width, height);
	}
	bool Device::CreateFactory()
	{
		//DXGIFactory作成
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory_));
		if (result == S_OK)
		{
			//ym::ConsoleLog("ファクトリー作成成功\n");
		}
#ifdef _DEBUG
		auto res = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,
			IID_PPV_ARGS(&pFactory_));
		if (res == S_OK)
		{
			ym::ConsoleLog("ファクトリー作成成功\n");
		}

#else

		auto res = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory_));
		if (res == S_OK)
		{
			ym::ConsoleLog("ファクトリー作成成功\n");
		}

#endif // _DEBUG
		return true;
	}
	bool Device::CreateDevice(ColorSpaceType csType)
	{
		{
			//=====================================================
			//アダプター作成
			//=====================================================
			size_t stringLength;
			DXGI_ADAPTER_DESC adapterDesc;
			unsigned int numModes = 0;
			IDXGIAdapter *adapter;
			int GPUNumber = 0;
			int GPUMaxMem = 0;
			IDXGIOutput *adapterOutput;
			{
				//一番強いGPUアダプタを検索
				for (int i = 0;; i++)
				{
					IDXGIAdapter *add;
					pFactory_->EnumAdapters(i, &add);
					if (add == nullptr)
					{
						break;
					}
					add->GetDesc(&adapterDesc);

					char videoCardDescription[128];
					//ビデオカード名を取得
					int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
					if (error != 0)
					{
						break;
					}
#ifdef _DEBUG


					cout << "ビデオカード名 : " << videoCardDescription << endl;

#endif // _DEBUG

					//ビデオカードメモリを取得（MB単位）
					float videoCardMemory = (float)(adapterDesc.DedicatedVideoMemory / 1024.f / 1024.f / 1024.f);

#ifdef _DEBUG

					cout << "ビデオメモリー : " << videoCardMemory << endl;
#endif // _DEBUG

					//アウトプット（モニター）に番号IDを付ける
					add->EnumOutputs(0, &adapterOutput);



					if (videoCardMemory > GPUMaxMem)
					{
						GPUMaxMem = videoCardMemory;
						GPUNumber = i;
					}
					add->Release();
					//アダプタアウトプットを解放
					if (adapterOutput != nullptr)
					{
						adapterOutput->Release();
						adapterOutput = 0;
					}
				}
			}
			pFactory_->EnumAdapters(GPUNumber, &adapter);

			//ディスプレイを列挙
	// ディスプレイを列挙する
			IDXGIOutput *pOutput{ nullptr };
			IDXGIOutput6 *pOutput_{ nullptr };
			int OutputIndex = 0;
			bool enableHDR = csType != ColorSpaceType::Rec709;
			while (adapter->EnumOutputs(OutputIndex, &pOutput) != DXGI_ERROR_NOT_FOUND)
			{
				auto hr = pOutput->QueryInterface(IID_PPV_ARGS(&pOutput_));
				SafeRelease(pOutput);
				if (FAILED(hr))
				{
					SafeRelease(pOutput_);
					continue;
				}

				// get desc1.
				DXGI_OUTPUT_DESC1 OutDesc;
				pOutput_->GetDesc1(&OutDesc);

				if (!enableHDR)
				{
					// if HDR mode disabled, choose first output.
					desktopCoordinates_ = OutDesc.DesktopCoordinates;
					minLuminance_ = OutDesc.MinLuminance;
					maxLuminance_ = OutDesc.MaxLuminance;
					maxFullFrameLuminance_ = OutDesc.MaxFullFrameLuminance;
					colorSpaceType_ = ColorSpaceType::Rec709;
					break;
				}

				if (OutDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
				{
					desktopCoordinates_ = OutDesc.DesktopCoordinates;
					minLuminance_ = OutDesc.MinLuminance;
					maxLuminance_ = OutDesc.MaxLuminance;
					maxFullFrameLuminance_ = OutDesc.MaxFullFrameLuminance;
					colorSpaceType_ = ColorSpaceType::Rec2020;
					break;
				}

				OutputIndex++;
			}
			//ディスプレイ情報をコンソールに書き出す
			{
				//ディスプレイ情報を取得
				DXGI_OUTPUT_DESC1 OutDesc;
				pOutput_->GetDesc1(&OutDesc);
				//ディスプレイ情報をコンソールに書き出す
				ym::ConsoleLog("ディスプレイ情報\n");
				ym::ConsoleLog("ディスプレイの座標\n");
				ym::ConsoleLog("左上X座標 : %d\n", OutDesc.DesktopCoordinates.left);
				ym::ConsoleLog("左上Y座標 : %d\n", OutDesc.DesktopCoordinates.top);
				ym::ConsoleLog("右下X座標 : %d\n", OutDesc.DesktopCoordinates.right);
				ym::ConsoleLog("右下Y座標 : %d\n", OutDesc.DesktopCoordinates.bottom);
				ym::ConsoleLog("最小輝度 : %f\n", OutDesc.MinLuminance);
				ym::ConsoleLog("最大輝度 : %f\n", OutDesc.MaxLuminance);
				ym::ConsoleLog("最大フレーム輝度 : %f\n", OutDesc.MaxFullFrameLuminance);
				ym::ConsoleLog("カラースペース : %d\n", OutDesc.ColorSpace);
			}
			SafeRelease(pOutput_);
			SafeRelease(pOutput);


			//フューチャーレベル　どのぐらいのマテリアル使えるか
			D3D_FEATURE_LEVEL levels[] =
			{
				D3D_FEATURE_LEVEL_12_1, // Direct3D 12.1  ShaderModel 6
				D3D_FEATURE_LEVEL_12_0, // Direct3D 12.0  ShaderModel 6
				D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
				D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
				D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
				D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
				D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3   ShaderModel 3
				D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2   ShaderModel 3
				D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1   ShaderModel 3
			};
			//Direct3Dデバイスの初期化
			D3D_FEATURE_LEVEL featureLevel;
			for (auto lv : levels)
			{
				//デバイス作ってる
				if (D3D12CreateDevice(adapter, lv, IID_PPV_ARGS(&pDevice_)) == S_OK)
				{
					featureLevel = lv;
					break;//生成可能なバージョンが見つかったらループを打ち切り
				}
			}

			pDevice_->QueryInterface(IID_PPV_ARGS(&pLatestDevice_));

			// COPY_DESCRIPTORS_INVALID_RANGESエラーを回避
			ID3D12InfoQueue *pD3DInfoQueue;
			if (SUCCEEDED(pDevice_->QueryInterface(__uuidof(ID3D12InfoQueue), reinterpret_cast<void **>(&pD3DInfoQueue))))
			{
#if 1
				// エラー等が出たときに止めたい場合は有効にする
				pD3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
				pD3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
				pD3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
#endif

				D3D12_MESSAGE_ID blockedIds[] = { D3D12_MESSAGE_ID_COPY_DESCRIPTORS_INVALID_RANGES };
				D3D12_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.pIDList = blockedIds;
				filter.DenyList.NumIDs = _countof(blockedIds);
				pD3DInfoQueue->AddRetrievalFilterEntries(&filter);
				pD3DInfoQueue->AddStorageFilterEntries(&filter);
				pD3DInfoQueue->Release();
			}

			adapter->Release();

			ym::ConsoleLog("デバイス作成成功\n");

			return true;
		}
	}
	bool Device::CreateCommandQueue()
	{
		//コマンドキュー作成
		pGraphicsQueue_ = std::make_unique<CommandQueue>();
		if (!pGraphicsQueue_)
		{
			return false;
		}
		if (!pGraphicsQueue_->Init(this, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_PRIORITY_HIGH))
		{
			return false;
		}
		ym::ConsoleLog("グラフィックキュー作成成功\n");
		return true;
	}
	bool Device::CreateFence()
	{
		//フェンス作成
		auto hr = pDevice_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence_));
		if (FAILED(hr))
		{
			return false;
		}
		fenceValue_ = 1;

		fenceEvent_ = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (fenceEvent_ == nullptr)
		{
			return false;
		}
		ym::ConsoleLog("フェンス作成成功\n");
		return true;
	}
	bool Device::CreateSwapChain(HWND hwnd, u32 width, u32 height)
	{
		//スワップチェイン作成


		pSwapChain_ = std::make_unique<SwapChain>();
		if (!pSwapChain_)
		{
			return false;
		}
		if (!pSwapChain_->Init(this, pGraphicsQueue_.get(), hwnd, width, height))
		{
			return false;
		}
		ym::ConsoleLog("スワップチェイン作成成功\n");
		return true;
	}
	bool Device::CreateDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 500000;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 1;
		pGlobalViewDescHeap_ = new GlobalDescriptorHeap();
		if (!pGlobalViewDescHeap_->Initialize(this, desc))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1000;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		pViewDescHeap_ = new DescriptorAllocator();
		if (!pViewDescHeap_->Initialize(this, desc))
		{
			return false;
		}


		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = 1000;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		pSamplerDescHeap_ = new DescriptorAllocator();
		if (!pSamplerDescHeap_->Initialize(this, desc))
		{
			return false;
		}



		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = 1000;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		pRtvDescHeap_ = new DescriptorAllocator();
		if (!pRtvDescHeap_->Initialize(this, desc))
		{
			return false;
		}
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = 1000;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		pDsvDescHeap_ = new DescriptorAllocator();
		if (!pDsvDescHeap_->Initialize(this, desc))
		{
			return false;
		}

		defaultViewDescInfo_ = pViewDescHeap_->Allocate();
		defaultSamplerDescInfo_ = pSamplerDescHeap_->Allocate();

		ym::ConsoleLog("ディスクリプタヒープ作成成功\n");

	}
}