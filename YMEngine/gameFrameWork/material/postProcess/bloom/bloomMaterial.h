#pragma once

#include "postProcess/postProcessMaterial.h"

namespace ym
{
	class Texture;
	class TextureView;
	class RenderTargetView;

	class Buffer;
	class ConstantBufferView;
	class VertexBufferView;
	class IndexBufferView;

	class Shader;
	class RootSignature;
	class GraphicsPipelineState;
	class DescriptorSet;
	class Sampler;

	struct BloomConstants
	{
		float bloomStrength_ = 1.0f;
		int bloomLevels_ = 4;
		float pad[2] = { 0,0 };
	};


	class BloomMaterial : public PostProcessMaterial
	{
	public:
		BloomMaterial() = default;
		~BloomMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void DrawImgui() override;
		void SetMaterial() override;
	private:
		void CreateShader();
		void CreatePipelineState();

		void CreateShrink();

		void UpdateBuffer();

		void CreateConstantBuffer();

	private:
		std::shared_ptr<Texture> shrinkTexture_;
		std::shared_ptr<TextureView> shrinkTexView_;
		std::shared_ptr<RenderTargetView> shrinkRtv_;

		std::shared_ptr<Shader> shrinkVS_;
		std::shared_ptr<Shader> shrinkPS_;

		std::shared_ptr<RootSignature> shrinkRootSignature_;
		std::shared_ptr<DescriptorSet> shrinkDescriptorSet_;
		std::shared_ptr<GraphicsPipelineState> shrinkPipelineState_;

		std::shared_ptr<Sampler> shrinksampler_;

		Vertex2D vertices_[4]
		{
			{DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
			{DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)},
			{DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
			{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)}
		};
		std::shared_ptr<Buffer> vertexBuffer_;
		std::shared_ptr<VertexBufferView> vertexBufferView_;
		u32 indices_[6]
		{
			0, 1, 2, 0, 2, 3
		};
		std::shared_ptr<Buffer> indexBuffer_;
		std::shared_ptr<IndexBufferView> indexBufferView_;

		BloomConstants constants_;
		BloomConstants *pConstants_{};

		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;

	};

}