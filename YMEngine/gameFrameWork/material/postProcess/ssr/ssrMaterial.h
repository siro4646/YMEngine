#pragma once
#include "postProcess/postProcessMaterial.h"

namespace ym
{
	struct SSRConstants
	{
		float thickness = 0.2f;
		float maxRayDistance = 50.0f;
		float stride = 1.0f;
		int maxRayMerchCount = 64;

		float baseReflectance;
		float padding[3]; // �p�f�B���O��16�o�C�g���E�ɍ��킹��
	};


	class Device;
	class CommandList;

	class Buffer;
	class ConstantBufferView;



	class SSRMaterial : public PostProcessMaterial
	{
	public:
		SSRMaterial() = default;
		~SSRMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void DrawImgui() override;
		void SetMaterial() override;
	private:
		void CreateShader();
		void CreatePipelineState();
		void CreateConstantBuffer();
		void UpdateBuffer();


	private:
		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;
		SSRConstants ssrConstants_;          // SSR�萔
		SSRConstants *pSSRConstants_ = nullptr; // �|�C���^�Œ萔���Q��


	};

}