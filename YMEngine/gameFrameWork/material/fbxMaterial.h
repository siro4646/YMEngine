#pragma once

#include "material.h"

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

	//���N���X
	class FBXMaterial: public Material
	{
	public:
		FBXMaterial() {}
		 ~FBXMaterial() {
			Uninit();
			Material::Uninit();
		}

		//�}�e���A�����Ƃ̏�����
		 void Uninit() override;
		 void Update()override;
		 void Draw()override;

		//�}�e���A���̐ݒ�

		void SetMaterial()override;		

	private:

		std::string maskMap_;
		std::shared_ptr<Texture> maskMapTex_;
		std::shared_ptr<TextureView> maskMapTexView_;
		std::string specularMap_;
		std::shared_ptr<Texture> specularMapTex_;
		std::shared_ptr<TextureView> specularMapTexView_;

		
		void LoadTexture()override;

		void CreateShader();

		void Init()override;

	};
}