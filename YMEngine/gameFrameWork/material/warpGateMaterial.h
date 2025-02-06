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
	class WarpGateMaterial : public Material
	{
	public:
		WarpGateMaterial() {}
		~WarpGateMaterial() {
			Uninit();
			Material::Uninit();
		}

		//�}�e���A�����Ƃ̏�����
		void Uninit() override;
		void Update()override;
		void Draw()override;

		//�}�e���A���̐ݒ�

		void SetMaterial()override;

		inline void SetWarpTexture(TextureView *texture) { warpTexture = texture; }


	private:
		TextureView *warpTexture;

		void CreateShader();
		void CreatePipelineState();

		void Init()override;

	};
}