#pragma once
#include "postProcess/postProcessMaterial.h"

namespace ym
{


	class Device;
	class CommandList;

	class Buffer;
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
		void SetMaterial() override;
	private:
		void CreateBuffer();
		void CreateShader();
		void CreatePipelineState();
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

	};

}