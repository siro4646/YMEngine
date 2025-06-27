#pragma once
#include "gameFrameWork/components/component.h"
#include <memory>
#include "gameFrameWork/material/material2.h"
namespace ym
{

	//描画系
	class Renderer;
	class Device;
	class CommandList;

	//後々マテリアルに移植するけどテストで
	//とりあえずMeshRendererに書く
	class Buffer;
	class ConstantBufferView;
	class Camera;

	class Shader;
	class RootSignature;
	class GraphicsPipelineState;
	class DescriptorSet;
	class Sampler;


	namespace mesh {
		class MeshInstance;
		class MeshFilter;

		class MeshRenderer : public Component
		{
		public:
			using Component::Component;


			void Init() override;
			void FixedUpdate() override {}
			void Update() override;
			void Draw() override;
			void Uninit() override;
			const char *GetName() const override { return "MeshRenderer"; }
			void DrawImguiBody() override; // ImGui表示

			inline void SetMeshFilter(MeshFilter *meshFilter) { meshFilter_ = meshFilter; }

			/* 追加するユーティリティ（任意） */
			void   SetMaterials(const std::vector<ym::material::Material *> &list) {
				//materials_ = list;
			}
			size_t GetMaterialCount()                   const { return materials_.size(); }
			ym::material::Material *GetMaterial(size_t i) const
			{
				if (i < materials_.size())
				{
					return materials_[i].get();
				}
				return nullptr;
			}
			void PushMaterial(std::shared_ptr<ym::material::Material> mat)
			{
				materials_.push_back(std::move(mat));
			}
			std::vector<std::shared_ptr<ym::material::Material>> GetMaterials() const
			{
				return materials_;
			}

		private:
			//===========================================================
			void BaseShader();
			void BasePipelineState();
			void BaseRootSignature();
			void BaseSampler();
			//===========================================================

		private:
			//===========================================================
			//シェーダー
			std::shared_ptr<Shader> vs_{};
			std::shared_ptr<Shader> ps_{};

			std::shared_ptr<RootSignature> rootSignature_{};
			std::shared_ptr<GraphicsPipelineState> pipelineState_{};
			std::shared_ptr<ym::Sampler> sampler_;
			std::shared_ptr<ym::DescriptorSet> descriptorSet_;
			std::vector<std::shared_ptr<ym::material::Material>> materials_;
			//ym::material::Material *material_ = nullptr; // マテリアルへの参照

			//===========================================================


			MeshFilter *meshFilter_ = nullptr; // MeshFilterへの参照
			Renderer *renderer_ = nullptr;
			Device *device_ = nullptr;
			CommandList *cmdList_ = nullptr;

		};
		REGISTER_COMPONENT(MeshRenderer);
	}

}
