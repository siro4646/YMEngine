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
	class ConstantBufferView;
	class VertexBufferView;
	class IndexBufferView;

	struct Mesh;

	//基底クラス
	class TornadoMaterial : public Material
	{
	public:
		TornadoMaterial() {}
		~TornadoMaterial() {
			Uninit();
			Material::Uninit();
		}

		//マテリアルごとの初期化
		void Uninit() override;
		void Update()override;
		void Draw()override;

		//マテリアルの設定

		void SetMaterial()override;

	private:
		std::string noiseMap_;
		std::shared_ptr<Texture> noiseMapTex_;
		std::shared_ptr<TextureView> noiseMapTexView_;

		//定数
		std::shared_ptr <ym::Buffer> constantBuffer_;
		std::shared_ptr <ym::ConstantBufferView> constBufferView_;

		struct SendData
		{
			float time;
			float dissolve;
			float padding[2];
		};

		SendData *pData_ = nullptr;
		SendData data_ = {};

		void LoadTexture()override;

		void CreateShader();

		void CreateConstantBuffer();

		void Init()override;

		bool isInit_ = false;
	};
}