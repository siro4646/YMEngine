#include "material.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "swapChain/swapChain.h"
#include "shader/shader.h"
#include "Renderer/renderer.h"
#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "texture/texture.h"
#include "textureView/textureView.h"

#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"

namespace ym
{
	void Material::Init()
	{
		descriptorSet_ = std::make_shared<DescriptorSet>();
		BaseShader();
		BaseRootSignature();
		BasePipelineState();
		BaseSampler();
		LoadTexture();
	}
	void Material::BaseShader()
	{
		//何も設定しない場合はunlitシェーダーを使用
		vs_ = std::make_shared<Shader>();
		vs_->Init(device_,ShaderType::Vertex,"unlit");
		ps_ = std::make_shared<Shader>();
		ps_->Init(device_, ShaderType::Pixel, "unlit");

		gs_ = std::make_shared<Shader>();
		hs_ = std::make_shared<Shader>();
		ds_ = std::make_shared<Shader>();

	}
	void Material::BasePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto &swapChain = device_->GetSwapChain();

		D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のPOSITION
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のNORMAL
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float2のTEXCOORD
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のTANGENT
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float4のCOLOR
		};

		ym::GraphicsPipelineStateDesc desc;
		desc.rasterizer.fillMode = D3D12_FILL_MODE_SOLID;
		desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
		desc.rasterizer.isFrontCCW = false;
		desc.rasterizer.isDepthClipEnable = false;
		desc.multisampleCount = 1;

		desc.blend.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		desc.blend.rtDesc[0].isBlendEnable = false;
		desc.blend.rtDesc[0].srcBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].dstBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpColor = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].srcBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].dstBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].writeMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.depthStencil.depthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.depthStencil.isDepthEnable = true;
		desc.depthStencil.isDepthWriteEnable = true;

		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vs_.get();
		desc.pPS = ps_.get();
		desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		//auto inputLayout = Vertex::InputLayout;
		desc.inputLayout.numElements = _countof(elementDescs);
		desc.inputLayout.pElements = elementDescs;
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = device_->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();
		desc.dsvFormat = device_->GetSwapChain().GetDepthStencilTexture()->GetTextureDesc().format;
		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		pipelineState_->Init(device_, desc);

	}
	void Material::BaseRootSignature()
	{
		rootSignature_ = std::make_shared<RootSignature>();
		rootSignature_->Init(device_, vs_.get(), ps_.get(), gs_.get(), hs_.get(), ds_.get());
	}
	void Material::BaseSampler()
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_ = std::make_shared<Sampler>();
		sampler_->Initialize(device_, desc);
	}
	void Material::Uninit()
	{
		vb_.reset();
		ib_.reset();
		vbView_.reset();
		ibView_.reset();
		vs_.reset();
		ps_.reset();
		gs_.reset();
		hs_.reset();
		ds_.reset();
		descriptorSet_.reset();
		rootSignature_.reset();
		pipelineState_.reset();
		sampler_.reset();
	}
	void Material::Update()
	{
	}
	void Material::Draw()
	{
		auto cmdList = commandList_->GetCommandList();
		commandList_->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &vbView_->GetView());
		cmdList->IASetIndexBuffer(&ibView_->GetView());
		cmdList->DrawIndexedInstanced(mesh_.Indices.size(), 1, 0, 0, 0);
	}
	void Material::SetMaterial()
	{
		auto cmdList = commandList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		auto ds = descriptorSet_.get();
		ds->Reset();
		ds->SetPsSrv(0, diffuseMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
	}
	void Material::SetVsCbv(u32 index, const D3D12_CPU_DESCRIPTOR_HANDLE &handle)
	{
		descriptorSet_->SetVsCbv(index, handle);
	}
	void Material::CreateMaterial(Mesh mesh)
	{
		mesh_ = mesh;
		renderer_ = Renderer::Instance();
		device_ = renderer_->GetDevice();
		commandList_ = renderer_->GetGraphicCommandList();

		CreateVertexBuffer();
		CreateIndexBuffer();

		Init();
	}
	void Material::CreateVertexBuffer()
	{
		vb_ = std::make_shared<Buffer>();
		vb_->Init(device_, mesh_.Vertices.size() * sizeof(Vertex3D), sizeof(Vertex3D), BufferUsage::VertexBuffer, false, false);
		vbView_ = std::make_shared<VertexBufferView>();
		vbView_->Init(device_, vb_.get());
		vb_->UpdateBuffer(device_, commandList_, mesh_.Vertices.data(), mesh_.Vertices.size() * sizeof(Vertex3D));
	}
	void Material::CreateIndexBuffer()
	{
		ib_ = std::make_shared<Buffer>();
		ib_->Init(device_, mesh_.Indices.size() * sizeof(u32), sizeof(u32), BufferUsage::IndexBuffer, false, false);
		ibView_ = std::make_shared<IndexBufferView>();
		ibView_->Init(device_, ib_.get());
		ib_->UpdateBuffer(device_, commandList_, mesh_.Indices.data(), mesh_.Indices.size() * sizeof(u32));
	}
	void Material::LoadTexture()
	{
		diffuseMap_ = ym::Utf16ToUtf8(mesh_.DiffuseMap);
		diffuseMapTex_ = std::make_shared<Texture>();
		diffuseMapTex_->LoadTexture(device_, commandList_, diffuseMap_.c_str());
		diffuseMapTexView_ = std::make_shared<TextureView>();
		diffuseMapTexView_->Init(device_, diffuseMapTex_.get());
	}
}
