#pragma once



namespace ym
{
	class Renderer;
	class Device;
	class CommandList;

	class Shader;
	class RootSignature;
	class GraphicsPipelineState;
	class DescriptorSet;
	class Sampler;
	class Texture;
	class TextureView;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;

	struct Mesh;

	class PSO
	{
	public:
		std::shared_ptr<Shader> vs_{};
		std::shared_ptr<Shader> ps_{};
		std::shared_ptr<Shader> gs_{};
		std::shared_ptr<Shader> hs_{};
		std::shared_ptr<Shader> ds_{};

		std::shared_ptr<RootSignature> rootSignature_{};
		std::shared_ptr<GraphicsPipelineState> pipelineState_{};
		std::shared_ptr<ym::Sampler> sampler_;
		std::shared_ptr<ym::DescriptorSet> descriptorSet_;
	};

	//基底クラス
	class Material
	{
	public:
		Material() {}
		virtual ~Material() {
			Uninit();
		}

		//マテリアルごとの初期化
		virtual void Uninit();
		virtual void Update();
		virtual void Draw();

		//マテリアルの設定

		virtual void SetMaterial();

		void SetVsCbv(u32 index, const D3D12_CPU_DESCRIPTOR_HANDLE &handle);

		void CreateMaterial(Mesh mesh);

	private:
		void CreateVertexBuffer();
		void CreateIndexBuffer();

		virtual void LoadTexture();

		virtual void Init();



	protected:
		void BaseShader();
		void BasePipelineState();
		void BaseRootSignature();
		void BaseSampler();

		Device *device_;
		CommandList *commandList_;
		Renderer *renderer_;

		std::string diffuseMap_;
		std::shared_ptr<Texture> diffuseMapTex_;
		std::shared_ptr<TextureView> diffuseMapTexView_;

		std::shared_ptr<Buffer> vb_;
		std::shared_ptr<VertexBufferView> vbView_;

		std::shared_ptr<Buffer> ib_;
		std::shared_ptr<IndexBufferView> ibView_;

		Mesh mesh_;

		std::shared_ptr<Shader> vs_{};
		std::shared_ptr<Shader> ps_{};
		std::shared_ptr<Shader> gs_{};
		std::shared_ptr<Shader> hs_{};
		std::shared_ptr<Shader> ds_{};

		std::shared_ptr<RootSignature> rootSignature_{};
		std::shared_ptr<GraphicsPipelineState> pipelineState_{};
		std::shared_ptr<ym::Sampler> sampler_;
		std::shared_ptr<ym::DescriptorSet> descriptorSet_;
	};
}