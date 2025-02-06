#include "debugMaterial.h"

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
	void DebugMaterial::Uninit()
	{		
		Material::Uninit();
	}
	void DebugMaterial::Update()
	{
	}
	void DebugMaterial::Draw()
	{
		Material::Draw();
	}
	void DebugMaterial::SetMaterial()
	{
		auto cmdList = commandList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		auto ds = descriptorSet_.get();
		ds->Reset();
	}
	void DebugMaterial::CreateShader()
	{
		vs_ = std::make_shared<Shader>();
		vs_->Init(device_, ShaderType::Vertex, "debug");
		ps_ = std::make_shared<Shader>();
		ps_->Init(device_, ShaderType::Pixel, "debug");

		gs_ = std::make_shared<Shader>();
		hs_ = std::make_shared<Shader>();
		ds_ = std::make_shared<Shader>();
	}
	void DebugMaterial::CreatePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto &swapChain = device_->GetSwapChain();

		ym::GraphicsPipelineStateDesc desc;
		desc = GraphicsPipelineState::GetDefaultDesc();
		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vs_.get();
		desc.pPS = ps_.get();
		desc.rasterizer.fillMode = D3D12_FILL_MODE_WIREFRAME;
		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		pipelineState_->Init(device_, desc);
	}
	void DebugMaterial::Init()
	{
		descriptorSet_ = std::make_shared<DescriptorSet>();
		CreateShader();
		BaseRootSignature();		
		BaseSampler();
		CreatePipelineState();

	}
}
