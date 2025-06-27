#include "dofMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"
#include "textureView/textureView.h"

#include "camera/cameraManager.h"
#include "camera/camera.h"


#include "renderTargetManager/renderTargetManager.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "blurTexture/blurTextureGenerator.h"

namespace ym
{
	void DoFMaterial::Init()
	{
		PostProcessMaterial::Init();
		isUsed_ = false;
		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();
		CreateConstantBuffer();


		//CreatePipelineState();
	}
	void DoFMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();

		constantBuffer_->Unmap();
		constantBuffer_.reset();
		constantBufferView_->Destroy();
		constantBufferView_.reset();
		

	}
	void DoFMaterial::Update()
	{
		PostProcessMaterial::Update();
		UpdateBuffer();

	}
	void DoFMaterial::Draw()
	{

		
	}
	void DoFMaterial::DrawImgui()
	{
		ImGui::Text("DoFMaterial");
		PostProcessMaterial::DrawImgui("DoFMaterial");
		ImGui::SliderFloat("Focus Distance", &dofConstants_.focusDistance, 0.0f, 1.0f);
		ImGui::SliderFloat("Focus Range", &dofConstants_.focusRange, 0.001f, 1.0f);
		ImGui::SliderFloat("Max Blur", &dofConstants_.maxBlur, 0.0f, 1.0f);
	}
	void DoFMaterial::SetMaterial()
	{

		//auto bbidx = device_->GetSwapChain().GetFrameIndex();
		//auto cmdList = graphicsCmdList_;
		//auto renderer = Renderer::Instance();

		//auto colorTex = pPM_->GetResultTexture(bbidx);
		//auto colorView = pPM_->GetResultTextureView(bbidx);
		//auto dsvTex = Renderer::Instance()->GetDepthStencilTexture();
		//auto dsvTexView = Renderer::Instance()->GetDepthStencilTexView();

		//auto camera = CameraManager::Instance().GetCamera("mainCamera");

		//cmdList->TransitionBarrier(colorTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		//cmdList->TransitionBarrier(dsvTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		//auto ds = descriptorSet_.get();

		//PostProcessMaterial::SetMaterial();

		//cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		//ds->Reset();
		//ds->SetPsSrv(0, colorView->GetDescInfo().cpuHandle);
		//ds->SetPsSrv(1, dsvTexView->GetDescInfo().cpuHandle);
		//ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		//ds->SetPsCbv(1, camera->GetDescriptorHandle());
		//cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), ds);


		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();

		auto colorTex = pPM_->GetResultTexture(bbidx);
		auto colorView = pPM_->GetResultTextureView(bbidx);
		auto dsvTex = Renderer::Instance()->GetDepthStencilTexture();
		auto dsvTexView = Renderer::Instance()->GetDepthStencilTexView();

		BlurTextureGenerator::Instance()->Draw(colorTex, colorView);

		auto blurTex = BlurTextureGenerator::Instance()->GetBlurTexture();
		auto blurView = BlurTextureGenerator::Instance()->GetBlurTextureView();
		auto camera = CameraManager::Instance().GetCamera("mainCamera");



		cmdList->TransitionBarrier(blurTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(dsvTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


		auto ds = descriptorSet_.get();

		PostProcessMaterial::SetMaterial();

		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		ds->Reset();
		ds->SetPsSrv(0,colorView->GetDescInfo().cpuHandle);
		ds->SetPsSrv(1, blurView->GetDescInfo().cpuHandle);
		ds->SetPsSrv(2, dsvTexView->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		ds->SetPsCbv(0, constantBufferView_->GetDescInfo().cpuHandle);
		ds->SetPsCbv(1, camera->GetDescriptorHandle());
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), ds);
	}
	void DoFMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "dof");
		ps_->Init(device_, ShaderType::Pixel, "dof");
		/*vs_->Init(device_, ShaderType::Vertex, "copy");
		ps_->Init(device_, ShaderType::Pixel, "copy");*/
	
	}

	void DoFMaterial::UpdateBuffer()
	{
		void *p = (DoFConstants *)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &dofConstants_, sizeof(dofConstants_));
			constantBuffer_->Unmap();
		}
	}

	void DoFMaterial::CreateConstantBuffer()
	{
		auto renderer = Renderer::Instance();

		constantBuffer_ = std::make_unique<Buffer>();
		constantBuffer_->Init(
			device_,
			sizeof(pDoFConstants_),
			sizeof(DoFConstants),
			BufferUsage::ConstantBuffer,
			true,
			false
		);
		constantBufferView_ = std::make_unique<ConstantBufferView>();
		constantBufferView_->Init(
			device_,
			constantBuffer_.get()
		);
		pDoFConstants_ = (DoFConstants *)constantBuffer_->Map();

		dofConstants_.focusDistance = 0.0f; // Default focus distance
		dofConstants_.focusRange = 0.1f; // Default focal length
		dofConstants_.maxBlur = 0.5f; // Default blur size
	}

}
