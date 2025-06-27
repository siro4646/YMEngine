#pragma once
#include "postProcess/postProcessMaterial.h"

#include "texture/texture.h"

namespace ym
{
	struct FXAAConstants
	{
		float fxaaThreshold;
		float fxaaBlendFactor;
		float fxaaSampleOffset;
		float padding;
	};


	class Device;
	class CommandList;

	class Buffer;

	class Sampler;

	class Buffer;
	class ConstantBufferView;
	class VertexBufferView;
	class IndexBufferView;


	class Texture;
	class TextureView;
	class RenderTargetView;

	class FXAAMaterial : public PostProcessMaterial
	{
	public:
		FXAAMaterial() = default;
		~FXAAMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void DrawImgui() override;
		void SetMaterial() override;
	private:
		void CreateShader();
		void UpdateBuffer();
		void CreateConstantBuffer();
	private:
		FXAAConstants fxaaConstants_;
		FXAAConstants *pFXAAConstants_;
		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;
	};

}