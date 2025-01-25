#include "pbrMaterial.h"

#include "shader/shader.h"
#include "texture/texture.h"
#include "textureView/textureView.h"
#include "descriptorSet/DescriptorSet.h"
#include "commandList/commandList.h"
#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"

namespace ym
{
	void PBRMaterial::Uninit()
	{
		vs_.reset();
		ps_.reset();
		gs_.reset();
		hs_.reset();
		ds_.reset();
		sampler_.reset();
		diffuseMapTex_.reset();
		diffuseMapTexView_.reset();
		maskMapTex_.reset();
		maskMapTexView_.reset();
		specularMapTex_.reset();
		specularMapTexView_.reset();
		Material::Uninit();
	}
	void PBRMaterial::Update()
	{
	}
	void PBRMaterial::Draw()
	{
		Material::Draw();
	}
	void PBRMaterial::SetMaterial()
	{
		auto cmdList = commandList_->GetCommandList();
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		auto ds = descriptorSet_.get();
		ds->Reset();
		ds->SetPsSrv(0, diffuseMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		ds->SetPsSrv(1, specularMapTexView_->GetDescInfo().cpuHandle);
		ds->SetPsSrv(2, maskMapTexView_->GetDescInfo().cpuHandle);

	}
	void PBRMaterial::LoadTexture()
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
	}
	void PBRMaterial::CreateShader()
	{
		vs_ = std::make_shared<Shader>();
		vs_->Init(device_, ShaderType::Vertex, "pbr");
		ps_ = std::make_shared<Shader>();
		ps_->Init(device_, ShaderType::Pixel, "pbr");

		gs_ = std::make_shared<Shader>();
		hs_ = std::make_shared<Shader>();
		ds_ = std::make_shared<Shader>();
	}
	void PBRMaterial::Init()
	{
		descriptorSet_ = std::make_shared<DescriptorSet>();
		CreateShader();
		BaseRootSignature();
		BasePipelineState();
		BaseSampler();
		LoadTexture();
	}
}
