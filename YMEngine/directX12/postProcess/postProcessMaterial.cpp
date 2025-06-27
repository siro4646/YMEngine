#include "postProcessMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "rootSignature/rootSignature.h"
#include "descriptorSet/descriptorSet.h"
#include "pipelineState/pipelineState.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "texture/texture.h"
#include "textureView/textureView.h"

#include "renderTargetManager/renderTargetManager.h"

#include "postProcess/postProcessManager.h"

namespace ym
{
	void PostProcessMaterial::Init()
	{
		device_ = Renderer::Instance()->GetDevice();
		graphicsCmdList_ = Renderer::Instance()->GetGraphicCommandList();
		pPM_ = PostProcessManager::Instance();

		rootSignature_ = std::make_shared<RootSignature>();
		descriptorSet_ = std::make_shared<DescriptorSet>();
		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		vs_ = std::make_shared<Shader>();
		ps_ = std::make_shared<Shader>();
		sampler_ = std::make_shared<Sampler>();

		texture_ = std::make_shared<Texture>();
		ym::TextureDesc texDesc; 
		texDesc = Renderer::Instance()->GetDevice()->GetSwapChain().GetCurrentTexture()->GetTextureDesc();
		texture_->Init(device_, texDesc);
		rtv_ = std::make_shared<RenderTargetView>();
		rtv_->Init(device_, texture_.get());
		textureView_ = std::make_shared<TextureView>();
		textureView_->Init(device_, texture_.get());


	}

	void PostProcessMaterial::Uninit()
	{
		device_ = nullptr;
		graphicsCmdList_ = nullptr;


		rootSignature_.reset();
		descriptorSet_.reset();
		pipelineState_.reset();
		vs_.reset();
		ps_.reset();
		sampler_.reset();

		texture_.reset();
		rtv_.reset();
		textureView_.reset();
	}

	void PostProcessMaterial::Update()
	{
	}

	void PostProcessMaterial::Draw()
	{
		
	}

	void PostProcessMaterial::DrawImgui()
	{
	}

	void PostProcessMaterial::DrawImgui(string guid)
	{
		ImGui::PushID(guid.c_str());
		ImGui::Checkbox("isUsed", &isUsed_);
		ImGui::PopID();
	}

	void PostProcessMaterial::SetMaterial()
	{
		auto cmdList = graphicsCmdList_;
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto rtm = RenderTargetManager::Instance();

		//テクスチャをRTVとして設定
		cmdList->TransitionBarrier(texture_.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		rtm->BeginRenderTarget(
			{ *rtv_ },
			{ *texture_ },
			nullptr,
			nullptr,
			true
		);
	}

	void PostProcessMaterial::BaseRootSignature()
	{
		rootSignature_->Init(device_, vs_.get(), ps_.get(), nullptr, nullptr,nullptr);
	}

	void PostProcessMaterial::BaseSampler()
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

		sampler_->Initialize(device_, desc);
	}

	void PostProcessMaterial::BasePipelineState()
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
