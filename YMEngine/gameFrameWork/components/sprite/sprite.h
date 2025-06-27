#pragma once

#include "gameFrameWork/components/component.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"
#include "shader/shader.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "sampler/sampler.h"
#include "texture/texture.h"
#include "textureView/textureView.h"

namespace ym
{	

	class Texture;

	class Buffer;
	class VertexBufferView;
	class Camera;

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	class Sprite :public Component
	{
	public:		
		using Component::Component;
		void Init() override;
		void FixedUpdate() override;
		void Update() override;
		void Draw() override;
		void Uninit() override;
		const char *GetName() const override { return "Sprite"; }

		void LoadTexture(const std::string &path);

	private:
		void CreateVertexBuffer();

		void CreateIndexBuffer();

		void CreateConstantBuffer();

		void UpdateMatrix();

		void CreatePipelineState();

		void CreateSampler();

	private:

		std::string texturePath_;
		std::shared_ptr<ym::Texture> texture_;
		std::shared_ptr<ym::TextureView> textureView_;

		Vertex				vertex_[4];
		std::shared_ptr <ym::Buffer> vertexBuffer_;
		std::shared_ptr <ym::VertexBufferView> vertexBufferView_;

		u32 			index_[6] = { 0,1,2,1,3,2 };
		std::shared_ptr<ym::Buffer>indexBuffer_{};
		std::shared_ptr<ym::IndexBufferView> indexBufferView_{};

		//íËêî
		std::shared_ptr <ym::Buffer> constantBuffer_;
		std::shared_ptr <ym::ConstantBufferView> constBufferView_;
		XMMATRIX *pMatrix_ = nullptr;

		Camera *pCamera_ = nullptr;

		XMFLOAT4 rect_ = XMFLOAT4(0, 0, 1, 1);
		XMFLOAT4 *pRect_ = nullptr;

		XMFLOAT4 color_ = XMFLOAT4(1, 1,1, 1);
		XMFLOAT4 *pColor_= nullptr;

		std::shared_ptr < ym::Shader> vertexShader_;
		std::shared_ptr < ym::Shader> pixelShader_;
		std::shared_ptr<ym::RootSignature> rootSignature_;
		std::shared_ptr<ym::GraphicsPipelineState> pipelineState_;
		std::shared_ptr<ym::DescriptorSet> descriptorSet_;
		std::shared_ptr<ym::Sampler> sampler_;

		bool isInit_ = false;
	};
	REGISTER_COMPONENT(Sprite);
}