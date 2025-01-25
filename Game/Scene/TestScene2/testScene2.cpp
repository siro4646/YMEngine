#include "testScene2.h"
#include "renderer/renderer.h"

#include "Game/Object/TestObject.h"
#include "Game/Object/TestObject2.h"


#include "gameFrameWork/requiredObject/mainCamera.h"
#include "rootSignature/rootSignature.h"
#include "shader/shader.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/descriptorSet.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"
#include "descriptorSet/descriptorSet.h"

#include "Renderer/renderer.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"

#include "application/application.h"

namespace ym
{
	void TestScene2::Init()
	{
		BaseScene::Init();

		ym::ConsoleLog("TestScene2::Init()\n");
		_renderer = Renderer::Instance();
		auto comCmdList = _renderer->GetComputeCommandList();
		//comCmdList->GetCommandList
		//_testObject->Init();
		_mainCamera = std::make_shared<MainCamera>();
		gameObjectManager_->AddGameObject(_mainCamera);

		CreateAsset();
		//_mainCamera->Init();
	}
	void TestScene2::UnInit()
	{
		BaseScene::UnInit();
		ym::ConsoleLog("TestScene2::UnInit()\n");

		_rootSignature.reset();
		_cs.reset();
		_computePipelineState.reset();
		_descriptorSet.reset();

		_inputBuffer.reset();
		_inputBufferView.reset();
		_outputBuffer.reset();
		_outputUAV.reset();
		_readBackBuffer.reset();





	}
	void TestScene2::FixedUpdate()
	{
		BaseScene::FixedUpdate();
	}
	void TestScene2::Update()
	{
		_renderer->Update();
		BaseScene::Update();
	}
	void TestScene2::Draw()
	{
		_renderer->BeginFrame();
		BaseScene::Draw();
		SendDataToGPU();

		_renderer->Draw();
		_renderer->EndFrame();
	}
	void TestScene2::CreateAsset()
	{
		auto device = Renderer::Instance()->GetDevice();
		_cs = std::make_shared<Shader>();
		_cs->Init(device, ShaderType::Compute, "cs");
		_rootSignature = std::make_shared<RootSignature>();
		_rootSignature->Init(device, _cs.get());
		_computePipelineState = std::make_shared<ComputePipelineState>();
		ComputePipelineStateDesc desc;
		desc.pRootSignature = _rootSignature.get();
		desc.pCS = _cs.get();
		_computePipelineState->Init(device, desc);

		_descriptorSet = std::make_shared<DescriptorSet>();



		// 入力データ
		inputData.resize(TESTCULCNUM);
		for (int i = 0; i < TESTCULCNUM; ++i)
		{
			inputData[i].data = i;
		}
		//outputData.resize(1024);

		size_t dataSize = inputData.size() * sizeof(TestData);
		_inputBuffer = std::make_shared<Buffer>();
		_inputBuffer->Init(device,dataSize, sizeof(TestData),BufferUsage::ShaderResource,false,true);
		_inputBufferView = std::make_shared<BufferView>();
		_inputBufferView->Init(device, _inputBuffer.get(), 0, inputData.size(), sizeof(TestData));
		_inputBuffer->UpdateBuffer(device, _renderer->GetGraphicCommandList(),inputData.data(), dataSize);

		_outputBuffer = std::make_shared<Buffer>();
		_outputBuffer->Init(device,dataSize, sizeof(float), BufferUsage::ShaderResource, false, true);
		_outputUAV = std::make_shared<UnorderedAccessView>();
		_outputUAV->Initialize(device, _outputBuffer.get(), 0, inputData.size(), sizeof(float), 0);

		_readBackBuffer = std::make_shared<Buffer>();
		_readBackBuffer->Init(device, dataSize, sizeof(float), BufferUsage::ReadBack, false, false);
		
	}
	void TestScene2::SendDataToGPU()
	{
		auto ds = _descriptorSet.get();
		auto comCmdList = _renderer->GetComputeCommandList();

		comCmdList->Reset();
		ds->Reset();

		// 入力バッファの状態を設定
		comCmdList->TransitionBarrier(_inputBuffer.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		// 出力バッファの状態を設定
		comCmdList->TransitionBarrier(_outputBuffer.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// デスクリプタセットを設定
		ds->SetCsSrv(0, _inputBufferView->GetDescInfo().cpuHandle);
		ds->SetCsUav(0, _outputUAV->GetDescInfo().cpuHandle);

		comCmdList->GetCommandList()->SetPipelineState(_computePipelineState->GetPSO());
		comCmdList->SetComputeRootSignatureAndDescriptorSet(_rootSignature.get(), ds);

		// Compute Shader の実行
		comCmdList->GetCommandList()->Dispatch(TESTCULCNUM / 256, 1, 1);

		// UAV 書き込みの完了を保証
		comCmdList->UAVBarrier(_outputBuffer.get());

		// 出力バッファを COPY_SOURCE に遷移
		comCmdList->TransitionBarrier(_outputBuffer.get(), D3D12_RESOURCE_STATE_COPY_SOURCE);

		// 出力バッファの内容を ReadBack バッファにコピー
		comCmdList->GetCommandList()->CopyResource(
			_readBackBuffer->GetResourceDep(),
			_outputBuffer->GetResourceDep()
		);

		comCmdList->Close();
		comCmdList->Execute();

		_renderer->GetDevice()->WaitForComputeCommandQueue();

		// ReadBack バッファからデータを取得
		float *resultData = reinterpret_cast<float *>(_readBackBuffer->Map());
		/*for (int i = 0; i < TESTCULCNUM; ++i) {
			std::cout << "Result[" << i << "] = " << resultData[i] << std::endl;
		}*/
		_readBackBuffer->Unmap();
	}

}
