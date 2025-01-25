#include "dissolveMaterial.h"

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
	void DissolveMaterial::Uninit()
	{

		constantBuffer_.reset();
		constBufferView_.reset();

		maskMapTex_.reset();
		maskMapTexView_.reset();
		specularMapTex_.reset();
		specularMapTexView_.reset();
		noiseMapTex_.reset();
		noiseMapTexView_.reset();

		Material::Uninit();
	}
	void DissolveMaterial::Update()
	{
		if (isInit_ == false) return;
		auto &input = KeyboardInput::GetInstance();
		if (input.GetKeyDown("8"))
		{
			time_.time += 0.1f;

		}
		if (input.GetKeyDown("7"))
		{
			//if (time_.time > 0.0f)
			time_.time -= 0.1f;

		}
		// マップしてデータを更新
		

		void *mappedData = (SendData *)constantBuffer_->Map();
		if (mappedData)
		{
			memcpy(mappedData, &time_, sizeof(time_));
			constantBuffer_->Unmap();
		}
	}
	void DissolveMaterial::Draw()
	{
		Material::Draw();
	}
	void DissolveMaterial::SetMaterial()
	{
		auto cmdList = commandList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		auto ds = descriptorSet_.get();
		ds->Reset();
		ds->SetPsSrv(0, diffuseMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		ds->SetPsSrv(1, specularMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSrv(2, maskMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSrv(3, noiseMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsCbv(0, constBufferView_->GetDescInfo().cpuHandle);

	}
	void DissolveMaterial::LoadTexture()
	{
		//テクスチャの読み込み
		diffuseMap_ = ym::Utf16ToUtf8(mesh_.DiffuseMap);
		diffuseMapTex_ = std::make_shared<Texture>();
		diffuseMapTex_->LoadTexture(device_, commandList_, diffuseMap_.c_str());
		diffuseMapTexView_ = std::make_shared<TextureView>();
		diffuseMapTexView_->Init(device_, diffuseMapTex_.get());
		maskMap_ = ym::Utf16ToUtf8(mesh_.MaskMap);
		maskMapTex_ = std::make_shared<Texture>();
		maskMapTex_->LoadTexture(device_, commandList_, maskMap_.c_str());
		maskMapTexView_ = std::make_shared<TextureView>();
		maskMapTexView_->Init(device_, maskMapTex_.get());

		specularMap_ = ym::Utf16ToUtf8(mesh_.SpecularMap);
		specularMapTex_ = std::make_shared<Texture>();
		specularMapTex_->LoadTexture(device_, commandList_, specularMap_.c_str());
		specularMapTexView_ = std::make_shared<TextureView>();
		specularMapTexView_->Init(device_, specularMapTex_.get());

		noiseMap_ = "asset/texture/noise.jpg";
		noiseMapTex_ = std::make_shared<Texture>();
		noiseMapTex_->LoadTexture(device_, commandList_, noiseMap_.c_str());
		noiseMapTexView_ = std::make_shared<TextureView>();
		noiseMapTexView_->Init(device_, noiseMapTex_.get());
	}
	void DissolveMaterial::CreateShader()
	{		
		vs_ = std::make_shared<Shader>();
		vs_->Init(device_, ShaderType::Vertex, "dissolveModel");
		ps_ = std::make_shared<Shader>();
		ps_->Init(device_, ShaderType::Pixel, "dissolveModel");

		gs_ = std::make_shared<Shader>();
		hs_ = std::make_shared<Shader>();
		ds_ = std::make_shared<Shader>();
	}
	void DissolveMaterial::CreateConstantBuffer()
	{
		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(device_, sizeof(*ptime_), sizeof(SendData), BufferUsage::ConstantBuffer, true, false);
		constBufferView_ = std::make_shared<ConstantBufferView>();
		constBufferView_->Init(device_, constantBuffer_.get());
		ptime_ = (SendData *)constantBuffer_->Map();
		ptime_ = &time_;		
	}
	void DissolveMaterial::Init()
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
