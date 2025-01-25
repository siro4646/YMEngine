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
	class DebugMaterial : public Material
	{
	public:
		DebugMaterial() {}
		~DebugMaterial() {
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
		void CreateShader();
		void CreatePipelineState();

		void Init()override;

	};
}