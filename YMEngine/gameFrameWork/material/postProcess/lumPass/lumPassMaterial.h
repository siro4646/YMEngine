#pragma once
#include "postProcess/postProcessMaterial.h"

namespace ym
{
	struct LumPassConstants
	{
		float threshold = 0.95f;
		float padding[3];
		float knee = 0.1f;
		float padding2[3];
	};

	class Device;
	class CommandList;

	class Buffer;
	class ConstantBufferView;
	class VertexBufferView;
	class IndexBufferView;

	class Shader;
	class RootSignature;
	class GraphicsPipelineState;



	class LumPassMaterial : public PostProcessMaterial
	{
	public:
		LumPassMaterial() = default;
		~LumPassMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void DrawImgui() override;
		void SetMaterial() override;
	private:
		void CreateBuffer();
		void CreateShader();
		void CreatePipelineState();
		void UpdateBuffer();

		void CreateConstantBuffer();
	private:
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

		std::shared_ptr<Shader> copyVS_;
		std::shared_ptr<Shader> copyPS_;

		std::shared_ptr<RootSignature> copyRootSignature_;
		std::shared_ptr<GraphicsPipelineState> copyPipelineState_;

		LumPassConstants constants_;
		LumPassConstants *pConstants_;

		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;

	};

}