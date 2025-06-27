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

	class Buffer;
	class VertexBufferView;

	namespace mesh {
		class MeshInstance;
		class MeshFilter;
		class Skeleton;

		class SkinnedMeshRenderer : public Component
		{
		public:
			using Component::Component;


			void Init() override;
			void FixedUpdate() override;
			void Update() override;
			void Draw() override;
			void Uninit() override;
			const char *GetName() const override { return "SkinnedMeshRenderer"; }
			void DrawImguiBody() override; // ImGui�\��

			void SetSkeleton(std::shared_ptr<Skeleton> skel);
			void SetMeshFilter(std::shared_ptr<MeshFilter> filter);

			void UpdateSkinning();

			void   SetMaterials(const std::vector<ym::material::Material *> &list) { 
				materials_.clear();
				for (auto &mat : list) {
					if (mat) {
						//materials_.push_back(std::make_shared<ym::material::Material>(mat));
					}
				}
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
			void SetMaterialCount(size_t count)
			{
				materials_.clear();
				for (int i = 0; i < count; ++i)
				{
					
				}
			}
			void PushMaterial(std::shared_ptr<ym::material::Material> mat)
			{
				materials_.push_back(std::move(mat));
			}

		private:
			void CreateBuffer();

		private:			

			std::shared_ptr<ym::DescriptorSet> descriptorSet_;
			std::vector<std::shared_ptr<ym::material::Material>> materials_;

			std::shared_ptr<MeshFilter> meshFilter_;  // ��MeshInstance���Q��
			std::shared_ptr<Skeleton> skeleton_;

			std::vector<XMFLOAT4X4> currentBoneMatrices_;  // CPU���ł̃A�j������
			std::shared_ptr<Buffer> skinnedVertexBuffer_; // GPU�p�X�L���ςݒ��_
			std::shared_ptr<VertexBufferView> skinnedVertexBufferView_; // GPU�p�X�L���ςݒ��_�r���[
			Renderer *renderer_ = nullptr;
			Device *device_ = nullptr;
			CommandList *cmdList_ = nullptr;

		};
		REGISTER_COMPONENT(SkinnedMeshRenderer);
	}

}
