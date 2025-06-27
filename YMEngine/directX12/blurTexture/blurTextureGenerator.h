#pragma once

namespace ym
{
	class Device;
	class CommandList;
	class Renderer;

	class Texture;
	class TextureView;
	class RenderTargetView;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;

	class Shader;
	class RootSignature;
	class GraphicsPipelineState;
	class DescriptorSet;
	class Sampler;

	class BlurTextureGenerator
	{
	public:
		BlurTextureGenerator() = default;
		~BlurTextureGenerator() = default;

		static BlurTextureGenerator *Instance()
		{
			static BlurTextureGenerator instance;
			return &instance;
		}

		void Init();
		void Uninit();
		void Draw(Texture* tex,TextureView* view);

		Texture *GetBlurTexture() { return blurTexture_.get(); }
		TextureView *GetBlurTextureView() { return blurTexView_.get(); }
		RenderTargetView *GetBlurRenderTargetView() { return blurRtv_.get(); }

	private:
		void CreateShader();
		void CreatePipelineState();
		void CreateBuffer();
		void CreateTexture();
	private:
		Device *device_{};
		CommandList *graphicsCmdList_{};
		Renderer *renderer_{};

		std::shared_ptr<Texture> bufTexture_;
		std::shared_ptr<TextureView> bufTexView_;
		std::shared_ptr<RenderTargetView> bufRtv_;

		std::shared_ptr<Texture> blurTexture_;
		std::shared_ptr<TextureView> blurTexView_;
		std::shared_ptr<RenderTargetView> blurRtv_;

		std::shared_ptr<Shader> blurVVS_;
		std::shared_ptr<Shader> blurVPS_;
		std::shared_ptr<Shader> blurHVS_;
		std::shared_ptr<Shader> blurHPS_;

		std::shared_ptr<RootSignature> rsV_;
		std::shared_ptr<RootSignature> rsH_;
		std::shared_ptr<DescriptorSet> ds_;
		std::shared_ptr<GraphicsPipelineState> psV_;
		std::shared_ptr<GraphicsPipelineState> psH_;
		std::shared_ptr<Sampler> sampler_;

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