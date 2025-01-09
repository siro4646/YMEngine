
#include "sprite.h"
#include "texture/texture.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "swapChain/swapChain.h"
#include "gameFrameWork/gameObject/gameObject.h"
#include "resource/resourceStateTracker.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"

namespace ym
{
	void Sprite::Init()
	{
		ym::ConsoleLog("Sprite::Init()\n");
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateConstantBuffer();
		CreateSampler();
		CreatePipelineState();
		descriptorSet_ = std::make_shared<DescriptorSet>();

		pCamera_ = CameraManager::Instance().GetMainCamera();


		/*pCmdList_->Close();
		pCmdList_->Execute();
		pDevice_->WaitForCommandQueue();
		pCmdList_->Reset();		*/


	}

	void Sprite::FixedUpdate()
	{
	}

	void Sprite::Update()
	{
		UpdateMatrix();
	}

	void Sprite::Draw()
	{
		if (!isInit_) return;
		auto rST = ResourceStateTracker::Instance();
		auto state = rST->GetState(texture_->GetResourceDep());


		auto cmdList = pCmdList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetVsCbv(0, constBufferView_->GetDescInfo().cpuHandle);
		descriptorSet_->SetVsCbv(1, pCamera_->GetDescriptorHandle());
		descriptorSet_->SetPsSrv(0, textureView_->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);

		pCmdList_->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
		cmdList->IASetIndexBuffer(&indexBufferView_->GetView());
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}

	void Sprite::Uninit()
	{
		vertexBuffer_.reset();
		indexBuffer_.reset();
		vertexBufferView_.reset();
		indexBufferView_.reset();
		constantBuffer_.reset();
		constBufferView_.reset();
		vertexShader_.reset();
		pixelShader_.reset();
		rootSignature_.reset();
		pipelineState_.reset();
		sampler_.reset();
		texture_.reset();
		textureView_.reset();
		descriptorSet_.reset();
		pMatrix_ = nullptr;
		isInit_ = false;
	}

	void Sprite::LoadTexture(const std::string &path)
	{
		texturePath_ = path;
		texture_ = std::make_shared<Texture>();
		texture_->LoadTexture(pDevice_.get(), pCmdList_.get(), path);
		textureView_ = std::make_shared<TextureView>();
		textureView_->Init(pDevice_.get(), texture_.get());
		auto rST = ResourceStateTracker::Instance();		
		isInit_ = true;
	}

	void Sprite::CreateVertexBuffer()
	{
		vertex_[0] = { XMFLOAT3(-1.f, 1.f, 0.0f),XMFLOAT2(0.0f, 0.0f) };//左上
		vertex_[1] = { XMFLOAT3(1.f, 1.f, 0.0f),XMFLOAT2(1.0f, 0.0f) };//右上
		vertex_[2] = { XMFLOAT3(-1.f, -1.f, 0.0f),XMFLOAT2(0.0f, 1.0f) };//左下
		vertex_[3] = { XMFLOAT3(1.f, -1.f, 0.0f),XMFLOAT2(1.0f, 1.0f) };//右下

		ym::ConsoleLog("頂点データのサイズ	%d\n", sizeof(Vertex));

		vertexBuffer_ = std::make_shared<Buffer>();
		vertexBuffer_.get()->Init(pDevice_.get(), sizeof(vertex_), sizeof(Vertex), BufferUsage::VertexBuffer, false, false);
		vertexBufferView_ = std::make_shared<VertexBufferView>();
		vertexBufferView_.get()->Init(pDevice_.get(), vertexBuffer_.get());

		vertexBuffer_.get()->UpdateBuffer(pDevice_.get(),pCmdList_.get(), &vertex_, sizeof(vertex_));
	}

	void Sprite::CreateIndexBuffer()
	{
		indexBuffer_ = std::make_shared<Buffer>();
		indexBuffer_.get()->Init(pDevice_.get(), sizeof(index_), sizeof(unsigned), BufferUsage::IndexBuffer, false, false);
		indexBufferView_ = std::make_shared<IndexBufferView>();
		indexBufferView_.get()->Init(pDevice_.get(), indexBuffer_.get());

		indexBuffer_.get()->UpdateBuffer(pDevice_.get(), pCmdList_.get(), &index_, sizeof(index_));
	}

	void Sprite::CreateConstantBuffer()
	{
		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_.get()->Init(pDevice_.get(), sizeof(*pMatrix_), sizeof(XMMATRIX), BufferUsage::ConstantBuffer, true, false);
		constBufferView_ = std::make_shared<ConstantBufferView>();
		constBufferView_.get()->Init(pDevice_.get(), constantBuffer_.get());

		pMatrix_ = (XMMATRIX *)constantBuffer_.get()->Map();
		if (!pMatrix_)
		{
			assert(0 && "定数バッファのマップに失敗");
		}
		*pMatrix_ = XMMatrixIdentity();
		pColor_ = reinterpret_cast<XMFLOAT4 *>(pMatrix_ + 1);
		*pColor_ = color_;
		pRect_ = reinterpret_cast<XMFLOAT4 *>(pColor_ + 1);
		*pRect_ = rect_;
		UpdateMatrix();
		/**pMatrix_ = XMMatrixIdentity();
		constantBuffer_.get()->Unmap();*/
	}

	void Sprite::UpdateMatrix()
	{
		Vector2 size = object->globalTransform.Scale.ToVector2();

		auto &sc = pDevice_->GetSwapChain();

		Vector2 windowSize = Vector2(sc.GetSize().x, sc.GetSize().y);

		Vector2 pos_ = object->globalTransform.Position.ToVector2();

		float x = pos_.x / windowSize.x * 0.5f;
		float y = pos_.y / windowSize.y * 0.5f;

		XMMATRIX pos = XMMatrixTranslation(x, y, 0.0f);
		XMMATRIX scale = XMMatrixScaling(size.x, size.y, 1.0f);
		XMMATRIX windowScale = XMMatrixScaling(1.0f / windowSize.x, 1.0f / windowSize.y, 1.0f);
		XMMATRIX rot = XMMatrixRotationZ(object->globalTransform.Rotation.z);
		XMMATRIX mat = scale * rot * windowScale * pos;
		(*pMatrix_) = XMMatrixTranspose(mat);
		(*pMatrix_) = object->globalTransform.GetMatrix();

	}

	void Sprite::CreatePipelineState()
	{
		vertexShader_ = std::make_shared<Shader>();
		vertexShader_->Init(pDevice_.get(),ShaderType::Vertex,"2dSprite");
		pixelShader_ = std::make_shared<Shader>();
		pixelShader_->Init(pDevice_.get(), ShaderType::Pixel, "2dSprite");
		rootSignature_ = std::make_shared<RootSignature>();
		rootSignature_->Init(pDevice_.get(), vertexShader_.get(), pixelShader_.get(),nullptr,nullptr,nullptr);
		auto bbidx = pDevice_->GetSwapChain().GetFrameIndex();

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
		desc.pVS = vertexShader_.get();
		desc.pPS = pixelShader_.get();
		desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		desc.inputLayout.numElements = _countof(elementDescs);
		desc.inputLayout.pElements = elementDescs;
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = pDevice_->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();

		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		pipelineState_->Init(pDevice_.get(), desc);

	}

	void Sprite::CreateSampler()
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_ = std::make_shared<Sampler>();
		sampler_->Initialize(pDevice_.get(), desc);
	}

}
