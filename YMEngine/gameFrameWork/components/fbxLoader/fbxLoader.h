#pragma once

#include "gameFrameWork/components/component.h"


	struct aiMesh;
	struct aiMaterial;

namespace ym
{
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
	class ConstantBufferView;

	class Camera;
	//struct Mesh;



		


	class FBXLoader :public Component
	{

	public:

	private:

	public:
		using Component::Component;
		void Init() override;
		void FixedUpdate() override;
		void Update() override;
		void Draw() override;
		void Uninit() override;

		bool Load(ImportSettings settings); // FBXファイルを読み込む

		//void LoadFBX(const std::string &path);
    private:
		void LoadMesh(Mesh &dst, const aiMesh *src, bool inverseU, bool inverseV);

		void NormalizeScale();

		void LoadTexture(const wchar_t *filename,Mesh &dst, const aiMaterial *src);

		void CreateVertexBuffer();
		void CreateIndexBuffer();

		void CreateConstantBuffer();

		void UpdateMatrix();

		void CreatePipelineState();

		void CreateSampler();

    private:
		std::vector<Mesh> meshes; // 出力先のメッシュ配列

		//std::vector<Vertex3D> vertices; // 頂点データの配列
		std::vector<std::shared_ptr<ym::Buffer>> vertexBuffers_;
		std::vector<std::shared_ptr<ym::VertexBufferView>> vertexBufferViews_;
		std::vector<std::shared_ptr<ym::Buffer>> indexBuffers_;
		std::vector<std::shared_ptr<ym::IndexBufferView>> indexBufferViews_;

		float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
		float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

		//定数
		std::shared_ptr <ym::Buffer> constantBuffer_;
		std::shared_ptr <ym::ConstantBufferView> constBufferView_;
		XMMATRIX *pMatrix_ = nullptr;

		Camera *pCamera_ = nullptr;

		int flags_ = 0;

		bool isInit_ = false;
		std::vector<std::string> texturePaths_;
		std::vector<std::shared_ptr<ym::Texture>> textures_;
		std::vector<std::shared_ptr<ym::TextureView>> textureViews_;

        std::shared_ptr<Shader> vsShader_{};
		std::shared_ptr<Shader> psShader_{};
		std::shared_ptr<RootSignature> rootSignature_{};
		std::shared_ptr<GraphicsPipelineState> pipelineState_{};
		std::shared_ptr<ym::DescriptorSet> descriptorSet_;
		std::shared_ptr<ym::Sampler> sampler_;

	};
}