#include "lumPassMaterial.h"
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

#include "renderTargetManager/renderTargetManager.h"

namespace ym
{
	void LumPassMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateBuffer();
		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		//PostProcessMaterial::BasePipelineState();
		CreatePipelineState();
	}
	void LumPassMaterial::Uninit()
	{
		vertexBufferView_.reset();
		vertexBuffer_.reset();
		indexBufferView_.reset();
		indexBuffer_.reset();



		copyVS_.reset();
		copyPS_.reset();
		copyRootSignature_.reset();
		copyPipelineState_.reset();
		PostProcessMaterial::Uninit();
	}
	void LumPassMaterial::Update()
	{
		PostProcessMaterial::Update();		
	}
	void LumPassMaterial::Draw()
	{
	}
	void LumPassMaterial::SetMaterial()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();
		auto colorTex = PostProcessManager::Instance()->GetResultTexture(bbidx);
		auto colorView = PostProcessManager::Instance()->GetResultTextureView(bbidx);
		auto lumTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance);
		auto lumView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::HighLuminance);
		auto lumRTV = renderer->GetSceneRenderTargetView(bbidx, MultiRenderTargets::HighLuminance);
		auto rtm = RenderTargetManager::Instance();

		//テクスチャをRTVとしてセット
		cmdList->TransitionBarrier(colorTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(lumTex, D3D12_RESOURCE_STATE_RENDER_TARGET);

		rtm->BeginRenderTarget(
			{ *lumRTV },
			{ *lumTex },
			nullptr,
			nullptr,
			true
		);
		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetPsSrv(0, colorView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());
		cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
		cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
		cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);	
		//texture_.swap(lumTex); = lumTex;

		/*texture_ = make_shared<Texture>(*lumTex);
		rtv_ = make_shared<RenderTargetView>(*lumRTV);
		textureView_ = make_shared<TextureView>(*lumView);*/

		//cmdList->TransitionBarrier(lumTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		
		PostProcessMaterial::SetMaterial();
		
		//テクスチャをシェーダーに渡すためにバリアを遷移
		cmdList->TransitionBarrier(colorTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);	
		cmdList->GetCommandList()->SetPipelineState(copyPipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetPsSrv(0, colorView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(copyRootSignature_.get(), descriptorSet_.get());


	}
	void LumPassMaterial::CreateBuffer()
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
	void LumPassMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "lumPass");
		ps_->Init(device_, ShaderType::Pixel, "lumPass");

		copyVS_ = std::make_shared<Shader>();
		copyVS_->Init(device_, ShaderType::Vertex, "copy");
		copyPS_ = std::make_shared<Shader>();
		copyPS_->Init(device_, ShaderType::Pixel, "copy");

	}
	void LumPassMaterial::CreatePipelineState()
	{
		auto renderer = Renderer::Instance();

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
		desc.rtvFormats[desc.numRTVs++] = renderer->GetSceneRenderTargetView(bbidx,MultiRenderTargets::Color)->GetFormat();
		pipelineState_->Init(device_, desc);

		copyRootSignature_ = std::make_shared<RootSignature>();
		copyRootSignature_->Init(device_, copyVS_.get(), copyPS_.get(), nullptr, nullptr, nullptr);

		copyPipelineState_ = std::make_shared<GraphicsPipelineState>();
		desc.pRootSignature = copyRootSignature_.get();
		desc.pVS = copyVS_.get();
		desc.pPS = copyPS_.get();
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = device_->GetSwapChain().GetCurrentTexture()->GetTextureDesc().format;
		copyPipelineState_->Init(device_, desc);
	}	
}
