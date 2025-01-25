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
	class DissolveMaterial: public Material
	{
	public:
		DissolveMaterial() {}
		 ~DissolveMaterial() {
			Uninit();
			Material::Uninit();
		}

		//マテリアルごとの初期化
		 void Uninit() override;
		 void Update()override;
		 void Draw()override;

		//マテリアルの設定

		void SetMaterial()override;		

		void SetTime(float time) {
			time_.time = time;			
		}

	private:

		std::string maskMap_;
		std::shared_ptr<Texture> maskMapTex_;
		std::shared_ptr<TextureView> maskMapTexView_;
		std::string specularMap_;
		std::shared_ptr<Texture> specularMapTex_;
		std::shared_ptr<TextureView> specularMapTexView_;

		std::string noiseMap_;
		std::shared_ptr<Texture> noiseMapTex_;
		std::shared_ptr<TextureView> noiseMapTexView_;

		//定数
		std::shared_ptr <ym::Buffer> constantBuffer_;
		std::shared_ptr <ym::ConstantBufferView> constBufferView_;

		struct SendData
		{
			float time;
			float padding[3];
		};

		SendData *ptime_ = nullptr;
		SendData time_ = {};
		
		void LoadTexture()override;

		void CreateShader();

		void CreateConstantBuffer();

		void Init()override;

		bool isInit_ = false;
	};
}