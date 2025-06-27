#pragma once
#include "postProcess/postProcessMaterial.h"

#include "texture/texture.h"

namespace ym
{
	struct DoFConstants
	{
		float focusDistance;
		float focusRange;
		float maxBlur;
		float padding; // アラインメント用
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

	class DoFMaterial : public PostProcessMaterial
	{
	public:
		DoFMaterial() = default;
		~DoFMaterial() = default;

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
		DoFConstants dofConstants_;
		DoFConstants *pDoFConstants_;
		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;
	};

}