#include "lightManager.h"



#include "device/device.h"
#include "Renderer/renderer.h"
#include "scene/sceneManager.h"

namespace ym
{
	void LightManager::Init()
	{
		CreatePointLightData();
		CreateBuffer();
	}
	void LightManager::Update()
	{
		UpdateBuffer();
	}
	void LightManager::Draw()
	{
	}
	void LightManager::CreatePointLightData()
	{
		//dumyPointLightData = new PointLightData();
		dumyPointLightData.position = { 0.0f,0.0f,0.0f };
		dumyPointLightData.color = { 1.0f,1.0f,1.0f };
		dumyPointLightData.intensity = 0.0f;
		dumyPointLightData.radius = 0.0f;
		pointLightDatas.push_back(dumyPointLightData);
	}
	void LightManager::CreateBuffer()
	{
		int CreateNum = 50;
		auto renderer = Renderer::Instance();
		auto device = renderer->GetDevice();
		auto dataSize = sizeof(PointLightData) * CreateNum;
		pointLightBuffer = std::make_shared<Buffer>();
		pointLightBuffer->Init(device, dataSize, sizeof(PointLightData), BufferUsage::ShaderResource,false,false);
		pointLightBufferView = std::make_shared<BufferView>();
		pointLightBufferView->Init(device,pointLightBuffer.get(),0, CreateNum,sizeof(PointLightData));
		pointLightBuffer->UpdateBuffer(device, renderer->GetGraphicCommandList(), pointLightDatas.data(), dataSize);

		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(device, sizeof(*pPointLightSceneData), sizeof(PointLightSceneData), BufferUsage::ConstantBuffer, true, false);
		constantBufferView_ = std::make_shared<ConstantBufferView>();
		constantBufferView_->Init(device, constantBuffer_.get());
		pPointLightSceneData = (PointLightSceneData *)constantBuffer_->Map();

		pointLightSceneData.num = pointLightNum;

		void *p = (PointLightSceneData *)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &pointLightSceneData, sizeof(pointLightSceneData));
			constantBuffer_->Unmap();
		}
		//UpdateBuffer();

	}
	void LightManager::UpdateBuffer()
	{
		auto renderer = Renderer::Instance();
		auto device = renderer->GetDevice();
		auto commandList = renderer->GetGraphicCommandList();
		auto dataSize = sizeof(PointLightData) * pointLightDatas.size();

		//更新前はコピー用にバリアを張る
		commandList->TransitionBarrier(pointLightBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST);

		pointLightBuffer->UpdateBuffer(device, renderer->GetGraphicCommandList(),pointLightDatas.data(), dataSize);

		//更新し終えたらshaderに渡すようにバリアを張る
		commandList->TransitionBarrier(pointLightBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


		void *p = (PointLightSceneData *)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &pointLightSceneData, sizeof(pointLightSceneData));
			constantBuffer_->Unmap();
		}
	}
}
