#pragma once

namespace ym
{
	class Device;
	class CommandList;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;
	struct Vertex2D;

	class Texture;
	class TextureView;
	class RenderTargetView;

	class RootSignature;
	class DescriptorSet;
	class GraphicsPipelineState;
	class Shader;
	class Sampler;

	class PostProcessManager;

	class PostProcessMaterial
	{
	public:

		virtual void Init();
		virtual void Uninit();
		virtual void Update();
		virtual void Draw();

		virtual void SetMaterial();



		TextureView *GetTextureView()
		{
			return textureView_.get();
		}
		Texture *GetTexture()
		{
			return texture_.get();
		}
		RenderTargetView *GetRTV()
		{
			return rtv_.get();
		}


	private:
	protected:
		Device *device_;
		CommandList *graphicsCmdList_;
		PostProcessManager *pPM_;




		std::shared_ptr<Texture> texture_;
		std::shared_ptr<TextureView> textureView_;
		std::shared_ptr<RenderTargetView> rtv_;

		std::shared_ptr<RootSignature> rootSignature_;
		std::shared_ptr<DescriptorSet> descriptorSet_;
		std::shared_ptr<GraphicsPipelineState> pipelineState_;
		std::shared_ptr<Shader> vs_;
		std::shared_ptr<Shader> ps_;
		std::shared_ptr<Sampler> sampler_;

		void BaseRootSignature();
		void BaseSampler();
		void BasePipelineState();
	};
}