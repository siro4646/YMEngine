#pragma once

#include "postProcess/postProcessMaterial.h"

namespace ym
{
	class GrayScaleMaterial : public PostProcessMaterial
	{
	public:
		GrayScaleMaterial() = default;
		~GrayScaleMaterial() = default;

		void Init() override;
		void Uninit() override;
		void Update() override;
		void Draw() override;
		void SetMaterial() override;
	private:
		void CreateShader();
		void CreatePipelineState();

	private:
	};

}