#pragma once
#include "postProcess/postProcessMaterial.h"

#include "texture/texture.h"

namespace ym
{
	enum class ToneMappingMode
	{
		ACESFilm,
		Lottes,
		Uncharted2,
		Reinhard,
		Linear
	};
	struct ToneMappingConstants
	{
		int mode = 0;
		float exposure = 1.0f;
		float padding[2] = { 0.0f, 0.0f };
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

	class ToonMapMaterial : public PostProcessMaterial
	{

	public:
		ToonMapMaterial() = default;
		~ToonMapMaterial() = default;

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
		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;
		ToneMappingConstants toneMappingConstants_;
		ToneMappingConstants *pToneMappingConstants_ = nullptr;
	};

}