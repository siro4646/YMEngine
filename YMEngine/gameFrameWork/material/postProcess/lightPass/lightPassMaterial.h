#pragma once
#include "postProcess/postProcessMaterial.h"

namespace ym
{

	struct DirectinalLight
	{
		XMFLOAT3 direction;
		float padding;
		XMFLOAT3 color;
		float padding2;
		float intensity;
		float padding3[3];
	};

	class Device;
	class CommandList;

	class Buffer;
	class ConstantBufferView;



	class LightPassMaterial : public PostProcessMaterial
	{
	public:
		LightPassMaterial() = default;
		~LightPassMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void DrawImgui() override;
		void SetMaterial() override;
	private:
		void CreateShader();
		void CreatePipelineState();

		void UpdateBuffer();

		void CreateConstantBuffer();
	private:
		Device *pDevice_;
		CommandList *pCommandList_;

		DirectinalLight light_;
		DirectinalLight *pLight_;
		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;

		float dir[3];
		float color[3];
		float intensity;


	};

}