#pragma once

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
	class Device;
	class CommandList;

	class Buffer;
	class VertexBufferView;

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	class Polygon
	{
	public:
		Polygon()
		{

		}
		~Polygon()
		{
			Destroy();
		}

		bool Init(Device *pDev,CommandList *cmdList);

		void Update();

		void Draw(CommandList *cmdList);

		void Destroy();

		// getter
		//const VertexBufferView &GetVertexBufferView() const { return vertexBufferView_[; }

	private:
		void CreateVertex(Device *pDev, CommandList *cmdList);

		void CreateIndex(Device *pDev, CommandList *cmdList);

		void CreateConstantBuffer(Device *pDev);

		void UpdateMatrix();

		void CreatePipelineState(Device *pDev);

		void CreateSampler(Device *pDev);

		void CreateTexture(Device *pDev, CommandList *cmdList);

		Vector2 size_ = Vector2(100, 100);
		Vector2 pos_ = Vector2(0, 0);
		float angle_ = 0;

		Vertex				vertex_[4];
		Buffer			vertexBuffer_;
		VertexBufferView vertexBufferView_;
		u32 			index_[6] = { 0,1,2,1,3,2 };
		Buffer			indexBuffer_{};
		IndexBufferView indexBufferView_{};

		//íËêî
		Buffer			constantBuffer_;
		ConstantBufferView constBufferView_;
		XMMATRIX *pMatrix_ = nullptr;

		Texture			texture_;
		TextureView		textureView_;

		Device *pDev_ = nullptr;
		CommandList *cmdList_ = nullptr;


		ym::Shader _vs;
		ym::Shader _ps;
		ym::RootSignature _rootSignature;
		ym::GraphicsPipelineState _pipelineState;
		ym::DescriptorSet _descriptorSet;
		ym::Sampler _sampler;

		bool isInit_ = false;



	};
}