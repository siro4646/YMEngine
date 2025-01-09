#include "mosaicMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"

namespace ym
{
	void MosaicMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();

		//CreatePipelineState();
	}
	void MosaicMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();
	}
	void MosaicMaterial::Update()
	{

	}
	void MosaicMaterial::Draw()
	{
		//auto tex = PostProcessManager::Instance()->GetPostProcessTexture();		
		//auto scTex = device_->GetSwapChain().GetCurrentTexture();

		/*descriptorSet_->Reset();
		descriptorSet_->SetPsSrv()*/

		//PostProcessMaterial::Draw();
	}
	void MosaicMaterial::SetMaterial()
	{
		PostProcessMaterial::SetMaterial();
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;

		auto tex = PostProcessManager::Instance()->GetResultTexture(bbidx);
		auto texView = PostProcessManager::Instance()->GetResultTextureView(bbidx);

		//テクスチャをシェーダーに渡すためにバリアを遷移
		cmdList->TransitionBarrier(tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetPsSrv(0, texView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());

	}
	void MosaicMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "mosaic");
		ps_->Init(device_, ShaderType::Pixel, "mosaic");
	}
	void MosaicMaterial::CreatePipelineState()
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
