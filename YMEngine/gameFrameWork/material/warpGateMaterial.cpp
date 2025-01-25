#include "warpGateMaterial.h"

#include "shader/shader.h"
#include "texture/texture.h"
#include "textureView/textureView.h"
#include "descriptorSet/DescriptorSet.h"
#include "commandList/commandList.h"
#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"

#include "Renderer/renderer.h"
#include "device/device.h"

namespace ym
{
	void WarpGateMaterial::Uninit()
	{
		Material::Uninit();
	}
	void WarpGateMaterial::Update()
	{
	}
	void WarpGateMaterial::Draw()
	{
		Material::Draw();
	}
	void WarpGateMaterial::SetMaterial()
	{
		auto cmdList = commandList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		auto ds = descriptorSet_.get();
		ds->Reset();
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		ds->SetPsSrv(0, warpTexture->GetDescInfo().cpuHandle);



	}
	void WarpGateMaterial::CreateShader()
	{
		vs_ = std::make_shared<Shader>();
		vs_->Init(device_, ShaderType::Vertex, "unlit");
		ps_ = std::make_shared<Shader>();
		ps_->Init(device_, ShaderType::Pixel, "unlit");

		gs_ = std::make_shared<Shader>();
		hs_ = std::make_shared<Shader>();
		ds_ = std::make_shared<Shader>();
	}
	void WarpGateMaterial::CreatePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto &swapChain = device_->GetSwapChain();

		ym::GraphicsPipelineStateDesc desc;
		desc = GraphicsPipelineState::GetDefaultDesc();
		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vs_.get();
		desc.pPS = ps_.get();
		desc.rasterizer.cullMode = D3D12_CULL_MODE_BACK;
		//desc.rasterizer.fillMode = D3D12_FILL_MODE_WIREFRAME;
		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		pipelineState_->Init(device_, desc);
	}
	void WarpGateMaterial::Init()
	{
		descriptorSet_ = std::make_shared<DescriptorSet>();
		CreateShader();
		BaseRootSignature();
		BaseSampler();
		CreatePipelineState();

	}
}
