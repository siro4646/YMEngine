#include "tornadoMaterial.h"

#include "shader/shader.h"
#include "texture/texture.h"
#include "textureView/textureView.h"
#include "descriptorSet/DescriptorSet.h"
#include "commandList/commandList.h"
#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"


namespace ym
{
	void TornadoMaterial::Uninit()
	{

		constantBuffer_.reset();
		constBufferView_.reset();
	
		noiseMapTex_.reset();
		noiseMapTexView_.reset();

		Material::Uninit();
	}
	void TornadoMaterial::Update()
	{
		if (isInit_ == false) return;
		auto &input = KeyboardInput::Instance();		
		// マップしてデータを更新
		data_.time += 0.03f;
		data_.dissolve = 0.65f;

		void *mappedData = (SendData *)constantBuffer_->Map();
		if (mappedData)
		{
			memcpy(mappedData, &data_, sizeof(data_));
			constantBuffer_->Unmap();
			
		}
	}
	void TornadoMaterial::Draw()
	{
		Material::Draw();
	}
	void TornadoMaterial::SetMaterial()
	{
		auto cmdList = commandList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		auto ds = descriptorSet_.get();
		ds->Reset();
		ds->SetPsSrv(0, diffuseMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);		
		ds->SetPsSrv(1, noiseMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsCbv(0, constBufferView_->GetDescInfo().cpuHandle);

	}
	void TornadoMaterial::LoadTexture()
	{
		//テクスチャの読み込み
		diffuseMap_ = ym::Utf16ToUtf8(mesh_.DiffuseMap);
		diffuseMapTex_ = std::make_shared<Texture>();
		diffuseMapTex_->LoadTexture(device_, commandList_, diffuseMap_.c_str());
		diffuseMapTexView_ = std::make_shared<TextureView>();
		diffuseMapTexView_->Init(device_, diffuseMapTex_.get());		

		noiseMap_ = "asset/texture/noise2.jpg";
		noiseMapTex_ = std::make_shared<Texture>();
		noiseMapTex_->LoadTexture(device_, commandList_, noiseMap_.c_str());
		noiseMapTexView_ = std::make_shared<TextureView>();
		noiseMapTexView_->Init(device_, noiseMapTex_.get());
	}
	void TornadoMaterial::CreateShader()
	{
		vs_ = std::make_shared<Shader>();
		vs_->Init(device_, ShaderType::Vertex, "tornado");
		ps_ = std::make_shared<Shader>();
		ps_->Init(device_, ShaderType::Pixel, "tornado");

		gs_ = std::make_shared<Shader>();
		hs_ = std::make_shared<Shader>();
		ds_ = std::make_shared<Shader>();
	}
	void TornadoMaterial::CreateConstantBuffer()
	{
		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(device_, sizeof(*pData_), sizeof(SendData), BufferUsage::ConstantBuffer, true, false);
		constBufferView_ = std::make_shared<ConstantBufferView>();
		constBufferView_->Init(device_, constantBuffer_.get());
		pData_ = (SendData *)constantBuffer_->Map();
		pData_ = &data_;
	}
	void TornadoMaterial::Init()
	{
		descriptorSet_ = std::make_shared<DescriptorSet>();
		CreateShader();
		BaseRootSignature();
		BasePipelineState();
		BaseSampler();
		LoadTexture();
		CreateConstantBuffer();

		isInit_ = true;

	}
}
