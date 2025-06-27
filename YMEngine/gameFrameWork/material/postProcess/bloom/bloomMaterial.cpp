#include "bloomMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "texture/texture.h"
#include "textureView/textureView.h"

#include "Renderer/renderer.h"

#include "renderTargetManager/renderTargetManager.h"

#include "blurTexture/blurTextureGenerator.h"

namespace ym
{
	void BloomMaterial::Init()
	{
		PostProcessMaterial::Init();
		isUsed_ = false;

		CreateShrink();


		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();
		CreateConstantBuffer();
		//CreatePipelineState();
	}
	void BloomMaterial::Uninit()
	{
		shrinkDescriptorSet_.reset();
		shrinkPipelineState_.reset();
		shrinkRootSignature_.reset();
		shrinkPS_.reset();
		shrinkVS_.reset();
		shrinkTexture_.reset();
		shrinkTexView_.reset();
		shrinkRtv_.reset();
		shrinksampler_.reset();

		vertexBufferView_.reset();
		vertexBuffer_.reset();
		indexBufferView_.reset();
		indexBuffer_.reset();

		constantBuffer_->Unmap();
		constantBuffer_.reset();
		constantBufferView_->Destroy();
		constantBufferView_.reset();

		PostProcessMaterial::Uninit();
	}
	void BloomMaterial::Update()
	{
		PostProcessMaterial::Update();
		UpdateBuffer();
	}
	void BloomMaterial::Draw()
	{
		//auto tex = PostProcessManager::Instance()->GetPostProcessTexture();		
		//auto scTex = device_->GetSwapChain().GetCurrentTexture();

		/*descriptorSet_->Reset();
		descriptorSet_->SetPsSrv()*/

		//PostProcessMaterial::Draw();
	}
	void BloomMaterial::DrawImgui()
	{
		ImGui::Text("BloomMaterial");
		PostProcessMaterial::DrawImgui("BloomMaterial");
		ImGui::SliderFloat("Bloom Strength", &constants_.bloomStrength_, 0.0f, 10.0f);
		ImGui::SliderInt("Bloom Levels", &constants_.bloomLevels_, 1, 10);
	}
	void BloomMaterial::SetMaterial()
	{
		
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto tex = PostProcessManager::Instance()->GetResultTexture(bbidx);
		auto texView = PostProcessManager::Instance()->GetResultTextureView(bbidx);
		auto lumTex = Renderer::Instance()->GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance);
		auto lumTexView = Renderer::Instance()->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::HighLuminance);

		auto rtm = RenderTargetManager::Instance();

		//テクスチャをRTVとして設定
		cmdList->TransitionBarrier(lumTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);		
		cmdList->TransitionBarrier(shrinkTexture_.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		rtm->BeginRenderTarget({*shrinkRtv_.get() },
			{*shrinkTexture_.get() },
			nullptr,
			nullptr,
			true
		);

		cmdList->GetCommandList()->SetPipelineState(shrinkPipelineState_->GetPSO());
		shrinkDescriptorSet_->Reset();
		shrinkDescriptorSet_->SetPsSrv(0, lumTexView->GetDescInfo().cpuHandle);
		shrinkDescriptorSet_->SetPsSampler(0, shrinksampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(shrinkRootSignature_.get(), shrinkDescriptorSet_.get());

		//縮小させる
		auto shrinkTexDesc = shrinkTexture_->GetResourceDep()->GetDesc();
		D3D12_VIEWPORT vp = {};
		D3D12_RECT sr = {};

		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;
		vp.Height = shrinkTexDesc.Height / 2;
		vp.Width = shrinkTexDesc.Width / 2;
		sr.top = 0;
		sr.left = 0;
		sr.right = vp.Width;
		sr.bottom = vp.Height;
		//描画
		cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
		cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
		
		for (int i = 0; i < constants_.bloomLevels_; i++)
		{

			cmdList->GetCommandList()->RSSetViewports(1, &vp);
			cmdList->GetCommandList()->RSSetScissorRects(1, &sr);
			cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
			sr.top += vp.Height;
			vp.TopLeftX = 0;
			vp.TopLeftY = sr.top;

			vp.Width /= 2;
			vp.Height /= 2;
			sr.bottom = sr.top + vp.Height;
		}
		//縮小テクスチャができたのでシェーダーリソースにする
		cmdList->TransitionBarrier(shrinkTexture_.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		//元のサイズに戻す
		Renderer::Instance()->SetViewPort();


		BlurTextureGenerator::Instance()->Draw(shrinkTexture_.get(), shrinkTexView_.get());

		auto blurTex = BlurTextureGenerator::Instance()->GetBlurTexture();
		auto blurView = BlurTextureGenerator::Instance()->GetBlurTextureView();


		PostProcessMaterial::SetMaterial();


		//テクスチャをシェーダーに渡すためにバリアを遷移
		cmdList->TransitionBarrier(tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		cmdList->TransitionBarrier(blurTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		//最終結果を書き込む


		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetPsSrv(0, blurView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(1, texView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsCbv(0, constantBufferView_->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());

	}
	void BloomMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "copy");
		ps_->Init(device_, ShaderType::Pixel, "bloom");
	}
	void BloomMaterial::CreatePipelineState()
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
	void BloomMaterial::CreateShrink()
	{
		auto render = Renderer::Instance();


		shrinkTexture_ = std::make_shared<Texture>();
		ym::TextureDesc texDesc;
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		texDesc = Renderer::Instance()->GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance)->GetTextureDesc();
		texDesc.clearColor[0] = 0.0f;
		texDesc.clearColor[1] = 0.0f;
		texDesc.clearColor[2] = 0.0f;
		texDesc.clearColor[3] = 0.0f;
		shrinkTexture_->Init(device_, texDesc);
		shrinkRtv_ = std::make_shared<RenderTargetView>();
		shrinkRtv_->Init(device_, shrinkTexture_.get());

		shrinkTexView_ = std::make_shared<TextureView>();
		shrinkTexView_->Init(device_, shrinkTexture_.get());

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

		//ディスクリプタセットの作成
		shrinkDescriptorSet_ = std::make_shared<DescriptorSet>();

		//シェーダーの作成
		shrinkVS_ = std::make_shared<Shader>();
		shrinkPS_ = std::make_shared<Shader>();
		shrinkVS_->Init(device_, ShaderType::Vertex, "shrink");
		shrinkPS_->Init(device_, ShaderType::Pixel, "shrink");

		//ルートシグネチャ
		shrinkRootSignature_ = std::make_shared<RootSignature>();
		shrinkRootSignature_->Init(device_, shrinkVS_.get(), shrinkPS_.get(), nullptr, nullptr, nullptr);

		//パイプラインステート
		{
			shrinkPipelineState_ = std::make_shared<GraphicsPipelineState>();
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

			desc.pRootSignature = shrinkRootSignature_.get();
			desc.pVS = shrinkVS_.get();
			desc.pPS = shrinkPS_.get();
			desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			desc.inputLayout.numElements = _countof(elementDescs);
			desc.inputLayout.pElements = elementDescs;
			desc.numRTVs = 0;
			desc.rtvFormats[desc.numRTVs++] = render->GetSceneRenderTexture(bbidx, MultiRenderTargets::HighLuminance)->GetTextureDesc().format;

			shrinkPipelineState_->Init(device_, desc);
		}

		//サンプラー
		{
			shrinksampler_ = std::make_shared<Sampler>();
			D3D12_SAMPLER_DESC desc{};
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			shrinksampler_->Initialize(device_, desc);
		}


	}
	void BloomMaterial::UpdateBuffer()
	{
		void *p = (BloomConstants *)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &constants_, sizeof(constants_));
			constantBuffer_->Unmap();
		}
	}
	void BloomMaterial::CreateConstantBuffer()
	{
		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(device_, sizeof(BloomConstants), sizeof(BloomConstants), BufferUsage::ConstantBuffer, true, false);
		constantBufferView_ = std::make_shared<ConstantBufferView>();
		constantBufferView_->Init(device_, constantBuffer_.get());	
		pConstants_ = &constants_;
		pConstants_->bloomStrength_ = 1;
		pConstants_->bloomLevels_ = 4;


		UpdateBuffer();
	}
}
