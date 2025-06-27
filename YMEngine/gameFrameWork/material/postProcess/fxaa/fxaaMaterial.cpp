#include "fxaaMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

//#include "texture/texture.h"
#include "textureView/textureView.h"


#include "renderTargetManager/renderTargetManager.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

namespace ym
{
	void FXAAMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();
		CreateConstantBuffer();


		//CreatePipelineState();
	}
	void FXAAMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();

		constantBuffer_->Unmap();
		constantBuffer_.reset();
		constantBufferView_->Destroy();
		constantBufferView_.reset();
		

	}
	void FXAAMaterial::Update()
	{
		PostProcessMaterial::Update();
		UpdateBuffer();
	}
	void FXAAMaterial::Draw()
	{

		
	}
	void FXAAMaterial::DrawImgui()
	{
		ImGui::Text("FXAAMaterial");
		PostProcessMaterial::DrawImgui("FXAAMaterial");
		ImGui::SliderFloat("FXAA Threshold", &fxaaConstants_.fxaaThreshold, 0.01f, 0.2f);
		ImGui::SliderFloat("FXAA Blend Factor", &fxaaConstants_.fxaaBlendFactor, 0.1f, 1.0f);
		ImGui::SliderFloat("FXAA Sample Offset", &fxaaConstants_.fxaaSampleOffset, 0.5f, 2.0f);
	}
	void FXAAMaterial::SetMaterial()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();

		auto colorTex = pPM_->GetResultTexture(bbidx);
		auto colorView = pPM_->GetResultTextureView(bbidx);

		auto ds = descriptorSet_.get();

		PostProcessMaterial::SetMaterial();

		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		ds->Reset();
		ds->SetPsSrv(0, colorView->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		ds->SetPsCbv(0, constantBufferView_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), ds);	
	}
	void FXAAMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "fxaa");
		ps_->Init(device_, ShaderType::Pixel, "fxaa");
	
	}

	void FXAAMaterial::UpdateBuffer()
	{
		void *p = (FXAAConstants *)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &fxaaConstants_, sizeof(fxaaConstants_));
			constantBuffer_->Unmap();
		}

	}

	void FXAAMaterial::CreateConstantBuffer()
	{
		auto renderer = Renderer::Instance();
		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(
			device_,
			sizeof(pFXAAConstants_),
			sizeof(FXAAConstants),
			BufferUsage::ConstantBuffer,
			true,
			false
		);
		constantBufferView_ = std::make_shared<ConstantBufferView>();
		constantBufferView_->Init(device_, constantBuffer_.get());
		pFXAAConstants_ = (FXAAConstants *)constantBuffer_->Map();

		fxaaConstants_.fxaaThreshold = 0.125f; // FXAA threshold
		fxaaConstants_.fxaaBlendFactor = 0.5f; // FXAA blend factor
		fxaaConstants_.fxaaSampleOffset = 1.0f; // FXAA sample offset
		UpdateBuffer();


	}

}
