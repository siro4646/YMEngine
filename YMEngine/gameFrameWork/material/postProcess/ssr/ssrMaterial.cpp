#include "ssrMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "camera/cameraManager.h"
#include "camera/camera.h"

#include "gameFrameWork/light/lightManager.h"

namespace ym
{
	void SSRMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();

		//CreatePipelineState();
	}
	void SSRMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();

	}
	void SSRMaterial::Update()
	{
		PostProcessMaterial::Update();

	}
	void SSRMaterial::Draw()
	{

	
	}
	void SSRMaterial::SetMaterial()
	{
		PostProcessMaterial::SetMaterial();

		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();

		auto colorTex = pPM_->GetResultTexture(bbidx);
		auto colorView = pPM_->GetResultTextureView(bbidx);
		auto normalTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::Normal);
		auto normalView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::Normal);
		auto worldPosTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::WorldPos);
		auto worldPosView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::WorldPos);

		auto dsvTex = renderer->GetDepthStencilTexture();
		auto dsvTexView = renderer->GetDepthStencilTexView();

		auto camera = CameraManager::Instance().GetCamera("mainCamera");

		cmdList->TransitionBarrier(colorTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(normalTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(dsvTex,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetPsSrv(0, colorView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(1, normalView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(2, worldPosView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(3, dsvTexView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsCbv(0, camera->GetDescriptorHandle());
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());


	}
	void SSRMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "ssr");
		ps_->Init(device_, ShaderType::Pixel, "ssr");
	}
	void SSRMaterial::CreatePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();

		D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		};
		ym::GraphicsPipelineStateDesc desc;
		desc.rasterizer.fillMode = D3D12_FILL_MODE_SOLID;
		desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
		desc.rasterizer.isFrontCCW = false;
		desc.rasterizer.isDepthClipEnable = false;
		desc.multisampleCount = 1;

		desc.blend.sampleMask = UINT_MAX;
		desc.blend.rtDesc[0].isBlendEnable = false;
		desc.blend.rtDesc[0].srcBlendColor = D3D12_BLEND_ONE;
		desc.blend.rtDesc[0].dstBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpColor = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].srcBlendAlpha = D3D12_BLEND_ONE;
		desc.blend.rtDesc[0].dstBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].writeMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.depthStencil.isDepthEnable = false;
		desc.depthStencil.isDepthWriteEnable = false;

		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vs_.get();
		desc.pPS = ps_.get();
		desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		desc.inputLayout.numElements = _countof(elementDescs);
		desc.inputLayout.pElements = elementDescs;
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = device_->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();

		pipelineState_->Init(device_, desc);

	}
}
