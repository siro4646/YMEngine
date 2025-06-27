#pragma once
#include "gameFrameWork/components/component.h"
#include <memory>
#include "gameFrameWork/material/material2.h"
namespace ym
{

	//�`��n
	class Renderer;
	class Device;
	class CommandList;

	//��X�}�e���A���ɈڐA���邯�ǃe�X�g��
	//�Ƃ肠����MeshRenderer�ɏ���
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
			void DrawImguiBody() override; // ImGui�\��

			inline void SetMeshFilter(MeshFilter *meshFilter) { meshFilter_ = meshFilter; }

			/* �ǉ����郆�[�e�B���e�B�i�C�Ӂj */
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
			//�V�F�[�_�[
			std::shared_ptr<Shader> vs_{};
			std::shared_ptr<Shader> ps_{};

			std::shared_ptr<RootSignature> rootSignature_{};
			std::shared_ptr<GraphicsPipelineState> pipelineState_{};
			std::shared_ptr<ym::Sampler> sampler_;
			std::shared_ptr<ym::DescriptorSet> descriptorSet_;
			std::vector<std::shared_ptr<ym::material::Material>> materials_;
			//ym::material::Material *material_ = nullptr; // �}�e���A���ւ̎Q��

			//===========================================================


			MeshFilter *meshFilter_ = nullptr; // MeshFilter�ւ̎Q��
			Renderer *renderer_ = nullptr;
			Device *device_ = nullptr;
			CommandList *cmdList_ = nullptr;

		};
		REGISTER_COMPONENT(MeshRenderer);
	}

}
