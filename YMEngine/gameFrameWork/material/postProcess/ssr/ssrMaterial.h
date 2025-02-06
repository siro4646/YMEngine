#pragma once
#include "postProcess/postProcessMaterial.h"

namespace ym
{



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
		void SetMaterial() override;
	private:
		void CreateShader();
		void CreatePipelineState();


	private:

	};

}