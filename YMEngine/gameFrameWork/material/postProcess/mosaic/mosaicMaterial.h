#pragma once

#include "postProcess/postProcessMaterial.h"

namespace ym
{
	class MosaicMaterial : public PostProcessMaterial
	{
	public:
		MosaicMaterial() = default;
		~MosaicMaterial() = default;

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