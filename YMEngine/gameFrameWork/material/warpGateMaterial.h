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

	//基底クラス
	class WarpGateMaterial : public Material
	{
	public:
		WarpGateMaterial() {}
		~WarpGateMaterial() {
			Uninit();
			Material::Uninit();
		}

		//マテリアルごとの初期化
		void Uninit() override;
		void Update()override;
		void Draw()override;

		//マテリアルの設定

		void SetMaterial()override;

		inline void SetWarpTexture(TextureView *texture) { warpTexture = texture; }


	private:
		TextureView *warpTexture;

		void CreateShader();
		void CreatePipelineState();

		void Init()override;

	};
}