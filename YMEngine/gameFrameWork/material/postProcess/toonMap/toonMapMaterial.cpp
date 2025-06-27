#include "toonMapMaterial.h"
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
	void ToonMapMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();
		CreateConstantBuffer();


		//CreatePipelineState();
	}
	void ToonMapMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();

		constantBuffer_->Unmap();
		constantBuffer_.reset();
		constantBufferView_->Destroy();
		constantBufferView_.reset();	
	}
	void ToonMapMaterial::Update()
	{
		PostProcessMaterial::Update();
		UpdateBuffer();
	}
	void ToonMapMaterial::Draw()
	{

		
	}
	void ToonMapMaterial::DrawImgui()
	{
		const char *toneMappingNames[] =
		{
			"ACES Film",
			"Lottes",
			"Uncharted2",
			"Reinhard",
			"Linear"
		};
		ImGui::Text("ToonMapMaterial");
		PostProcessMaterial::DrawImgui("ToonMapMaterial");
		int toneMappingIndex = static_cast<int>(toneMappingConstants_.mode);

		if (ImGui::Combo("Tone Mapping Mode", &toneMappingIndex, toneMappingNames, IM_ARRAYSIZE(toneMappingNames)))
		{
			toneMappingConstants_.mode = toneMappingIndex;
		}
		ImGui::SliderFloat("Exposure", &toneMappingConstants_.exposure, 0.0f,5.0f);

	}
	void ToonMapMaterial::SetMaterial()
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
	void ToonMapMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "toonMapping");
		ps_->Init(device_, ShaderType::Pixel, "toonMapping");
	
	}

	void ToonMapMaterial::UpdateBuffer()
	{
		void *p = (ToneMappingConstants *)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &toneMappingConstants_, sizeof(toneMappingConstants_));
			constantBuffer_->Unmap();
		}

	}

	void ToonMapMaterial::CreateConstantBuffer()
	{
		auto renderer = Renderer::Instance();
		device_ = renderer->GetDevice();
		graphicsCmdList_ = renderer->GetGraphicCommandList();

		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(device_,sizeof(*pToneMappingConstants_),sizeof(ToneMappingConstants), BufferUsage::ConstantBuffer, true, false);
		constantBufferView_ = std::make_shared<ConstantBufferView>();
		constantBufferView_->Init(device_, constantBuffer_.get());
		pToneMappingConstants_ = (ToneMappingConstants *)constantBuffer_->Map();

		toneMappingConstants_.mode = static_cast<int>(ToneMappingMode::ACESFilm);
		toneMappingConstants_.exposure = 1.0f;
		UpdateBuffer();


	}

}
