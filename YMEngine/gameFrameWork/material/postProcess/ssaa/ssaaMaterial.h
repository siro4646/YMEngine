#pragma once
#include "postProcess/postProcessMaterial.h"

#include "texture/texture.h"

namespace ym
{



	class Device;
	class CommandList;

	class Buffer;

	class Sampler;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;


	class Texture;
	class TextureView;
	class RenderTargetView;

	class SSAAMaterial : public PostProcessMaterial
	{
	public:
		SSAAMaterial() = default;
		~SSAAMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void SetMaterial() override;
	private:
		void CreateSSAATexture();
		void CreateShader();

		void CreateBuffer();

		void CreateReadbackBuffer();

		void ScreenShot();
	private:
		//======
		ComPtr<ID3D12Resource> readbackBuffer;
		//======

		std::shared_ptr<Sampler> linearSampler_;

		std::shared_ptr<Texture> ssaaTexture_;
		std::shared_ptr<TextureView> ssaaTextureView_;
		std::shared_ptr<RenderTargetView> ssaaRTV_;

		Texture copyTexture_{};

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

	};

}