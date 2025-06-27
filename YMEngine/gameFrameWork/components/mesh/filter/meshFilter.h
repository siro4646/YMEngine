#pragma once
#include "gameFrameWork/components/component.h"
#include <memory>
namespace ym
{
	//ÉoÉbÉtÉ@ån
	class Buffer;
	class VertexBufferView;
	class IndexBufferView;
	class ConstantBufferView;
	class Camera;


	//ï`âÊån
	class Renderer;
	class Device;
	class CommandList;
	class DescriptorSet;

	namespace mesh {
		class MeshInstance;
		class MeshRenderer;


		class MeshFilter : public Component
		{
		public:
			using Component::Component;

			void Init() override;
			void FixedUpdate() override {}
			void Update() override;
			void Draw() override {}
			void Uninit() override;
			const char *GetName() const override { return "MeshFilter"; }

			void SetMeshInstance(std::shared_ptr<MeshInstance> instance);
			std::shared_ptr<MeshInstance> GetMeshInstance() const;

			void DrawImguiBody() override; // ImGuiï\é¶
			void ApplyBuffers(VertexBufferView * vb = nullptr, IndexBufferView * iv = nullptr);


			void ApplyConstantBuffers(DescriptorSet *ds);

		private:

			void CreateVertexBuffer();
			void CreateIndexBuffer();
			void CreateConstantBuffer();
			void UpdateMatrix();
		private:
			std::shared_ptr<MeshInstance> meshInstance_;

			std::shared_ptr<Buffer> vertexBuffer_;

			std::shared_ptr<VertexBufferView> vbView_;

			std::shared_ptr<Buffer> indexBuffer_;

			std::shared_ptr<IndexBufferView> ibView_;

			//íËêî
			std::shared_ptr <ym::Buffer> constantBuffer_;
			std::shared_ptr <ym::ConstantBufferView> constBufferView_;
			XMMATRIX *pMatrix_ = nullptr;
			Camera *pCamera_ = nullptr;

			Renderer *renderer_ = nullptr;
			Device *device_ = nullptr;
			CommandList *cmdList_ = nullptr;

		};
	}
}
