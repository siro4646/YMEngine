#include "blurTextureGenerator.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "texture/texture.h"
#include "textureView/textureView.h"

#include "Renderer/renderer.h"

#include "renderTargetManager/renderTargetManager.h"

namespace ym
{
	void BlurTextureGenerator::Init()
	{
		renderer_ = Renderer::Instance();
		device_ = renderer_->GetDevice();
		graphicsCmdList_ = renderer_->GetGraphicCommandList();

		CreateShader();
		CreatePipelineState();
		CreateBuffer();
		CreateTexture();


	}
	void BlurTextureGenerator::Uninit()
	{
		bufTexture_.reset();
		bufTexView_.reset();
		bufRtv_.reset();

		blurTexture_.reset();
		blurTexView_.reset();
		blurRtv_.reset();

		blurVVS_.reset();
		blurVPS_.reset();
		blurHVS_.reset();
		blurHPS_.reset();

		rsV_.reset();
		rsH_.reset();

		psV_.reset();
		psH_.reset();

		sampler_.reset();

		ds_.reset();

		vertexBuffer_.reset();
		vertexBufferView_.reset();
		indexBuffer_.reset();
		indexBufferView_.reset();
	}
	void BlurTextureGenerator::Draw(Texture *tex, TextureView *view)
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();

		auto rtm = RenderTargetManager::Instance();


		//横方向のぼかし
		//バリア
		cmdList->TransitionBarrier(tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(bufTexture_.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		//レンダーターゲットの設定
		rtm->PushRenderTarget({ *bufRtv_.get() }, {*bufTexture_.get() }, nullptr, nullptr, false);

		//パイプラインステートの設定
		cmdList->GetCommandList()->SetPipelineState(psV_->GetPSO());
		ds_->Reset();
		ds_->SetPsSrv(0, view->GetDescInfo().cpuHandle);
		ds_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rsV_.get(), ds_.get());

		cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
		cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
		cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
		rtm->PopRenderTarget();

		//縦方向のぼかし
		//バリア
		cmdList->TransitionBarrier(bufTexture_.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(blurTexture_.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		//レンダーターゲットの設定
		rtm->PushRenderTarget({ *blurRtv_.get() }, { *blurTexture_.get() }, nullptr, nullptr, false);

		//パイプラインステートの設定
		cmdList->GetCommandList()->SetPipelineState(psH_->GetPSO());
		ds_->Reset();
		ds_->SetPsSrv(0, bufTexView_->GetDescInfo().cpuHandle);
		ds_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rsH_.get(), ds_.get());

		cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
		cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
		cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
		rtm->PopRenderTarget();
	}
	void BlurTextureGenerator::CreateShader()
	{
		blurVVS_ = std::make_shared<Shader>();
		blurVVS_->Init(device_, ShaderType::Vertex, "copy");
		blurVPS_ = std::make_shared<Shader>();
		blurVPS_->Init(device_, ShaderType::Pixel, "blurV");

		blurHVS_ = std::make_shared<Shader>();
		blurHVS_->Init(device_, ShaderType::Vertex, "copy");
		blurHPS_ = std::make_shared<Shader>();
		blurHPS_->Init(device_, ShaderType::Pixel, "blurH");
	}
	void BlurTextureGenerator::CreatePipelineState()
	{
		rsV_ = std::make_shared<RootSignature>();
		rsV_->Init(device_, blurVVS_.get(),blurVPS_.get(), nullptr, nullptr, nullptr);
		rsH_ = std::make_shared<RootSignature>();
		rsH_->Init(device_, blurHVS_.get(), blurHPS_.get(), nullptr, nullptr, nullptr);

		//横用パイプライン
		{
			psV_ = std::make_shared<GraphicsPipelineState>();
			GraphicsPipelineStateDesc desc;
			auto bbidx = device_->GetSwapChain().GetFrameIndex();
			D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

			};
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

			desc.pRootSignature = rsV_.get();
			desc.pVS = blurVVS_.get();
			desc.pPS = blurVPS_.get();
			desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			desc.inputLayout.numElements = _countof(elementDescs);
			desc.inputLayout.pElements = elementDescs;
			desc.numRTVs = 0;
			desc.rtvFormats[desc.numRTVs++] = renderer_->GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance)->GetTextureDesc().format;

			psV_->Init(device_, desc);
		}

		//縦用パイプライン
		{
			psH_ = std::make_shared<GraphicsPipelineState>();
			GraphicsPipelineStateDesc desc;
			auto bbidx = device_->GetSwapChain().GetFrameIndex();
			D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

			};
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

			desc.pRootSignature = rsH_.get();
			desc.pVS = blurHVS_.get();
			desc.pPS = blurHPS_.get();
			desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			desc.inputLayout.numElements = _countof(elementDescs);
			desc.inputLayout.pElements = elementDescs;
			desc.numRTVs = 0;
			desc.rtvFormats[desc.numRTVs++] = renderer_->GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance)->GetTextureDesc().format;

			psH_->Init(device_, desc);
		}

		//サンプラー
		{
			sampler_ = std::make_shared<Sampler>();
			D3D12_SAMPLER_DESC desc{};
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler_->Initialize(device_, desc);
		}

		ds_ = std::make_shared<DescriptorSet>();

	}
	void BlurTextureGenerator::CreateBuffer()
	{
		//頂点バッファの作成		
		vertexBuffer_ = std::make_shared<Buffer>();
		vertexBuffer_->Init(device_, sizeof(vertices_), sizeof(Vertex2D), BufferUsage::VertexBuffer, false, false);
		vertexBufferView_ = std::make_shared<VertexBufferView>();
		vertexBufferView_->Init(device_, vertexBuffer_.get());
		vertexBuffer_->UpdateBuffer(device_, graphicsCmdList_, &vertices_, sizeof(vertices_));

		//インデックスバッファの作成
		indexBuffer_ = std::make_shared<Buffer>();
		indexBuffer_->Init(device_, sizeof(indices_), sizeof(u32), BufferUsage::IndexBuffer, false, false);
		indexBufferView_ = std::make_shared<IndexBufferView>();
		indexBufferView_->Init(device_, indexBuffer_.get());
		indexBuffer_->UpdateBuffer(device_, graphicsCmdList_, &indices_, sizeof(indices_));
	}

	void BlurTextureGenerator::CreateTexture()
	{
		bufTexture_ = std::make_shared<Texture>();
		ym::TextureDesc texDesc;
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		texDesc = Renderer::Instance()->GetSceneRenderTexture(bbidx, MultiRenderTargets::Color)->GetTextureDesc();
		texDesc.clearColor[0] = 0.0f;
		texDesc.clearColor[1] = 0.0f;
		texDesc.clearColor[2] = 0.0f;
		texDesc.clearColor[3] = 0.0f;
		bufTexture_->Init(device_, texDesc);

		bufTexView_ = std::make_shared<TextureView>();
		bufTexView_->Init(device_, bufTexture_.get());

		bufRtv_ = std::make_shared<RenderTargetView>();
		bufRtv_->Init(device_, bufTexture_.get());

		blurTexture_ = std::make_shared<Texture>();
		blurTexture_->Init(device_, texDesc);

		blurTexView_ = std::make_shared<TextureView>();
		blurTexView_->Init(device_, blurTexture_.get());

		blurRtv_ = std::make_shared<RenderTargetView>();
		blurRtv_->Init(device_, blurTexture_.get());

	}


}