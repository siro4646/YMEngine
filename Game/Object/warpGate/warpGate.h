#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {

	class DissolveMaterial;

	class PBRMaterial;

	class TornadoMaterial;

	class Camera;

	class MainCamera;

	class RenderTexture;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;
	class ConstantBufferView;

	struct Vertex3D;

	class RootSignature;
	class DescriptorSet;
	class GraphicsPipelineState;
	class Shader;
	class Sampler;



	//コピー用テンプレート
	class WarpGate : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		void SetCameraName(string name) { cameraName = name; }

	private:
		void CreateBuffer();

		void CreatePSO();

		void UpdateMatrix();

		static int createNum;

		Vertex3D				vertex_[4];
		std::shared_ptr <Buffer> vertexBuffer_;
		std::shared_ptr <VertexBufferView> vertexBufferView_;

		u32 			index_[6] = { 0,1,2,1,3,2 };
		std::shared_ptr<Buffer>indexBuffer_{};
		std::shared_ptr<IndexBufferView> indexBufferView_{};

		Camera *camera;

		//定数
		std::shared_ptr <Buffer> constantBuffer_;
		std::shared_ptr <ConstantBufferView> constBufferView_;
		XMMATRIX *pMatrix_ = nullptr;
		
		string cameraName;

		int renderCount = 0;

		std::shared_ptr<RenderTexture> renderTexture;

		std::shared_ptr<Shader>vs_;
		std::shared_ptr<Shader>ps_;
		std::shared_ptr<RootSignature>rS_;
		std::shared_ptr<GraphicsPipelineState>pS;
		std::shared_ptr<DescriptorSet>descriptorSet_;
		std::shared_ptr<Sampler>sampler_;

	};
}