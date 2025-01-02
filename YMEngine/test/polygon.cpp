#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"
#include "polygon.h"

#include "device/device.h"
#include "commandList/commandList.h"

#include "swapChain/swapChain.h"

#include "utility/fileStream/fileStream.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"




bool ym::Polygon::Init(Device *pDev, CommandList *cmdList)
{
	pDev_ = pDev;
	cmdList_ = cmdList;
	CreateVertex(pDev, cmdList);

	CreateIndex(pDev, cmdList);

	CreateConstantBuffer(pDev);

	CreateSampler(pDev);

	CreatePipelineState(pDev);

	CreateTexture(pDev, cmdList);

	string name = "test.png";

	//ym::ConsoleLog("width:%d height:%d\n", a.width, a.height);


	/*cmdList->Close();
	cmdList->Execute();
	pDev->WaitForCommandQueue();
	cmdList->Reset();*/

	isInit_ = true;
	return true;

}

void ym::Polygon::Update()
{
	auto &input = ym::KeyboardInput::GetInstance();

	if (input.GetKeyDown("F"))
	{
		size_.x += 10;
		size_.y += 10;

	}
	else if (input.GetKeyDown("G"))
	{
		size_.x -= 10;
		size_.y -= 10;
		
	}
	else if (input.GetKeyDown("R"))
	{
		angle_ += 0.1f;
	}
	else if (input.GetKeyDown("T"))
	{
		angle_ -= 0.1f;
	}
	else if (input.GetKeyDown("UP"))
	{
		pos_.y += 10;
	}
	else if (input.GetKeyDown("DOWN"))
	{
		pos_.y -= 10;
	}
	else if (input.GetKeyDown("LEFT"))
	{
		pos_.x -= 10;
	}
	else if (input.GetKeyDown("RIGHT"))
	{
		pos_.x += 10;
	}

	UpdateMatrix();

}

void ym::Polygon::Draw(CommandList *cmdList)
{
	if (isInit_ == false)
	{
		return;
	}

	auto pCmdList = cmdList->GetCommandList();

	pCmdList->SetPipelineState(_pipelineState.GetPSO());

	_descriptorSet.Reset();
	_descriptorSet.SetVsCbv(0, constBufferView_.GetDescInfo().cpuHandle);
	_descriptorSet.SetPsSrv(0, textureView_.GetDescInfo().cpuHandle);
	_descriptorSet.SetPsSampler(0, _sampler.GetDescInfo().cpuHandle);

	cmdList->SetGraphicsRootSignatureAndDescriptorSet(&_rootSignature, &_descriptorSet);

	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0,1, &vertexBufferView_.GetView());
	pCmdList->IASetIndexBuffer(&indexBufferView_.GetView());
	pCmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void ym::Polygon::Destroy()
{
	vertexBuffer_.Destroy();
	indexBuffer_.Destroy();

	_vs.Destroy();
	_ps.Destroy();
	_rootSignature.Destroy();
	_pipelineState.Destroy();

	_sampler.Destroy();

	//texture_.Destroy();

	textureView_.Destroy();




	ym::ConsoleLog("Destroy\n");
}

void ym::Polygon::CreateVertex(Device *pDev, CommandList *cmdList)
{
	vertex_[0] = { XMFLOAT3(-1.f,1, 0.0f), XMFLOAT2(0.0f, 0.0f) };//左上
	vertex_[1] = { XMFLOAT3(1.f,1, 0.0f), XMFLOAT2(1.0f, 0.0f) };//右上
	vertex_[2] = { XMFLOAT3(-1, -1, 0.0f), XMFLOAT2(0.0f, 1.0f) };//左下
	vertex_[3] = { XMFLOAT3(1, -1, 0.0f), XMFLOAT2(1.0f, 1.0f) };//右下

	if (!vertexBuffer_.Init(pDev, sizeof(vertex_), sizeof(Vertex), BufferUsage::VertexBuffer, false, false))
	{
		assert(0 && "頂点バッファの生成に失敗");
	}
	if (!vertexBufferView_.Init(pDev, &vertexBuffer_))
	{
		assert(0 && "頂点バッファビューの生成に失敗");
	}

	vertexBuffer_.UpdateBuffer(pDev,cmdList,&vertex_, sizeof(vertex_));
	ym::ConsoleLog("CreateVertex\n");
}

void ym::Polygon::CreateIndex(Device *pDev, CommandList *cmdList)
{

	if (!indexBuffer_.Init(pDev, sizeof(index_), sizeof(unsigned), BufferUsage::IndexBuffer, false, false))
	{
		assert(0 && "インデックスバッファの生成に失敗");
	}
	if (!indexBufferView_.Init(pDev, &indexBuffer_))
	{
		assert(0 && "インデックスバッファビューの生成に失敗");
	}

	indexBuffer_.UpdateBuffer(pDev, cmdList, &index_, sizeof(index_));

	ym::ConsoleLog("CreateIndex\n");
}

void ym::Polygon::CreateConstantBuffer(Device *pDev)
{
	if (!constantBuffer_.Init(pDev, sizeof(*pMatrix_), sizeof(XMMATRIX), BufferUsage::ConstantBuffer, true, false))
	{
	assert(0 && "定数バッファの生成に失敗");
	}

	if (!constBufferView_.Init(pDev, &constantBuffer_))
	{
		assert(0 && "定数バッファビューの生成に失敗");
	}


	pMatrix_ = (XMMATRIX *)constantBuffer_.Map();
	if (!pMatrix_)
	{
		assert(0 && "定数バッファのマップに失敗");
	}
	UpdateMatrix();
//(*pMatrix_) = XMMatrixIdentity();
//constantBuffer_.Unmap();
//ym::ConsoleLog("CreateConstantBuffer\n");
}

void ym::Polygon::UpdateMatrix()
{
	float scale_x = size_.x;
	float scale_y = size_.y;

	auto &sc = pDev_->GetSwapChain();

	float window_width = sc.GetSize().x;
	float window_height = sc.GetSize().y;

	float position_x = pos_.x;
	float position_y = pos_.y;

	float x = (static_cast<float>(position_x)) / static_cast<float>(window_width) * 0.5f;
	float y = (static_cast<float>(-position_y)) / static_cast<float>(window_height) * 0.5f;

	XMMATRIX pos = XMMatrixTranslation(x, y, 0.0f);
	XMMATRIX scale = XMMatrixScaling(scale_x, scale_y, 1.0f);
	XMMATRIX windowScale = XMMatrixScaling(1 / window_width, 1 / window_height, 1.0f);
	XMMATRIX rot = XMMatrixRotationZ(angle_);
	XMMATRIX mat = scale * rot * windowScale * pos;
	(*pMatrix_) = XMMatrixTranspose(mat);
}

void ym::Polygon::CreatePipelineState(Device *pDev)
{
	//モザイク
	if (!_vs.Init(pDev, ShaderType::Type::Vertex, "test"))
	{
		assert(0 && "シェーダーの初期化に失敗");
	}
	if (!_ps.Init(pDev, ShaderType::Type::Pixel, "test"))
	{
		assert(0 && "シェーダーの初期化に失敗");
	}

	//if (!_vs.Init(pDev, ShaderType::Type::Vertex, "asset/shader/test/cso/testVS.cso"))
	//{
	//	assert(0 && "シェーダーの初期化に失敗");
	//}
	//if (!_ps.Init(pDev, ShaderType::Type::Pixel, "asset/shader/test/cso/testPS.cso"))
	//{
	//	assert(0 && "シェーダーの初期化に失敗");
	//}



	if (!_rootSignature.Init(pDev, &_vs,& _ps, nullptr, nullptr, nullptr))
	{
		assert(0 && "ルートシグネチャの初期化に失敗");
	}
	auto bbidx = pDev->GetSwapChain().GetFrameIndex();

	D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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

	desc.pRootSignature = &_rootSignature;
	desc.pVS = &_vs;
	desc.pPS = &_ps;
	desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	desc.inputLayout.numElements = _countof(elementDescs);
	desc.inputLayout.pElements = elementDescs;
	desc.numRTVs = 0;
	desc.rtvFormats[desc.numRTVs++] = pDev->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();
	if (!_pipelineState.Init(pDev, desc))
	{
		assert(0 && "パイプラインステートの初期化に失敗");
	}


}

void ym::Polygon::CreateSampler(Device *pDev)
{
	D3D12_SAMPLER_DESC desc{};
	desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	if (!_sampler.Initialize(pDev, desc))
	{
		assert(0 && "サンプラーの初期化に失敗");
	}
}

void ym::Polygon::CreateTexture(Device *pDev, CommandList *cmdList)
{
	//テクスチャの生成	
	if (!texture_.LoadTexture(pDev, cmdList,"asset/texture/test.png"))
	{
		assert(0 && "テクスチャの初期化に失敗");
	}

	if (!textureView_.Init(pDev, &texture_))
	{
		assert(0 && "テクスチャビューの初期化に失敗");
	}
	//バリアを張る
//	cmdList->TransitionBarrier(&texture_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


}
