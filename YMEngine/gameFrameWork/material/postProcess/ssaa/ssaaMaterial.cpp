#include "ssaaMaterial.h"
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
	void SSAAMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateShader();
		CreateBuffer();
		CreateSSAATexture();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();



		//CreatePipelineState();
	}
	void SSAAMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();

		ssaaTextureView_.reset();
		ssaaTexture_.reset();
		ssaaRTV_.reset();

		vertexBuffer_.reset();
		vertexBufferView_.reset();
		indexBuffer_.reset();
		indexBufferView_.reset();


	}
	void SSAAMaterial::Update()
	{
		PostProcessMaterial::Update();
		auto &kb = KeyboardInput::Instance();

	}
	void SSAAMaterial::Draw()
	{
		
	}
	void SSAAMaterial::SetMaterial()
	{
		//シーンのテクスチャを高解像度で描画
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;

		auto tex = PostProcessManager::Instance()->GetResultTexture(bbidx);
		auto texView = PostProcessManager::Instance()->GetResultTextureView(bbidx);

		auto rtm = RenderTargetManager::Instance();

		auto ds = descriptorSet_.get();

		//バリアの設定
		cmdList->TransitionBarrier(tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(ssaaTexture_.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		rtm->BeginRenderTarget(
			{ *ssaaRTV_},
			{ *ssaaTexture_},
			nullptr,
			nullptr,
			true
		);

		//SSAAテクスチャに描画
		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		ds->Reset();
		ds->SetPsSrv(0, texView->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), ds);

		//拡大させる
		auto shrinkTexDesc = ssaaTexture_->GetResourceDep()->GetDesc();
		D3D12_VIEWPORT vp = {};
		D3D12_RECT sr = {};

		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;
		vp.Height = shrinkTexDesc.Height;
		vp.Width = shrinkTexDesc.Width;
		sr.top = 0;
		sr.left = 0;
		sr.right = vp.Width;
		sr.bottom = vp.Height;
		

		//描画
		cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
		cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
		cmdList->GetCommandList()->RSSetViewports(1, &vp);
		cmdList->GetCommandList()->RSSetScissorRects(1, &sr);
		cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

		//高解像度RTVをテクスチャに戻す
		cmdList->TransitionBarrier(ssaaTexture_.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		copyTexture_ = *ssaaTexture_.get();

		//元のサイズに戻す
		Renderer::Instance()->SetViewPort();

		PostProcessMaterial::SetMaterial();

		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		ds->Reset();
		ds->SetPsSrv(0, ssaaTextureView_->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, linearSampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), ds);
	}
	void SSAAMaterial::CreateSSAATexture()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();
		auto sceneTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::Color);
		auto sceneTexView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::Color);
		ssaaTexture_ = std::make_shared<Texture>();
		TextureDesc texDesc;
		texDesc = sceneTex->GetTextureDesc();
		texDesc.width = texDesc.width * 2;
		texDesc.height = texDesc.height * 2;
		ssaaTexture_->Init(device_, texDesc);
		ssaaTextureView_ = std::make_shared<TextureView>();
		ssaaTextureView_->Init(device_, ssaaTexture_.get());
		ssaaRTV_ = std::make_shared<RenderTargetView>();
		ssaaRTV_->Init(device_, ssaaTexture_.get());

		copyTexture_ = *ssaaTexture_.get();
	}
	void SSAAMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "copy");
		ps_->Init(device_, ShaderType::Pixel, "copy");

		linearSampler_ = std::make_shared<Sampler>();
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.BorderColor[0] = 1.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D12_FLOAT32_MAX;

		linearSampler_->Initialize(device_, desc);
	}
	void SSAAMaterial::CreateBuffer()
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
}
