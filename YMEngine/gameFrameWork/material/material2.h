#pragma once

#include "shader/shader.h"

namespace ym
{
	class Shader;
	enum class ShaderVariableType;
	struct ShaderConstantBuffer;
	struct ShaderVariable;
	struct ShaderReflectionInfo;
	class Buffer;
	class ConstantBufferView;
	class Device;
	class CommandList;
    class Camera;
	class CameraManager;


    class RootSignature;
    class GraphicsPipelineState;
    class DescriptorSet;
    class Sampler;

	class Texture;
	class TextureView;


	namespace material
	{
        struct MaterialConstantBufferInfo
        {
            bool useExternalBuffer = false; // �O�����L���H
            std::shared_ptr<Buffer> buffer; // GPU �o�b�t�@�i�K�v���j
			D3D12_CPU_DESCRIPTOR_HANDLE cpuAddress = {}; // CPU���̃A�h���X�i���L�o�b�t�@�̏ꍇ�j
            UINT slot = 0;
            std::vector<uint8_t> cpuData; // �� CPU���̏������ݗp�f�[�^
			bool dirty = false; // CPU���̃f�[�^���ύX���ꂽ���ǂ���
            std::shared_ptr<ConstantBufferView> cbv;
        };
        struct MaterialTextureInfo {
            std::shared_ptr<Texture> texture;
			std::shared_ptr<TextureView> textureView; // �e�N�X�`���r���[
            u32 slot; // t0, t1...�ɑΉ�
			bool isInitialized = false; // �������ς݂��ǂ���
        };


        class Material
        {
        public:
            void Init();
			void Init(const string &vsFileName, const string &psFileName); // �V�F�[�_�[�̏�����
            void SetFloat(const std::string &name, float value);
			void SetFloat2(const std::string &name, const DirectX::XMFLOAT2 &value);
			void SetFloat3(const std::string &name, const DirectX::XMFLOAT3 &value);
			void SetFloat4(const std::string &name, const DirectX::XMFLOAT4 &value);
            void SetMatrix(const std::string &name, const DirectX::XMMATRIX &matrix);

			void SetTexture(const std::string &name, std::shared_ptr<Texture> texture);
			void SetTexture(const std::string &name, const std::string filePath);
            void Update();
            void ApplyConstantBuffer(DescriptorSet *ds);
            void DrawImGui();

        private:
			void InitConstantBuffer();

            void DrawConstantBuffersImGui();   
            void DrawTexturesImGui();          
            void DrawShaderSettingsImGui();   

			void UnInitBuffer();

            void BasePipelineState();
            void BaseRootSignature();
            void BaseSampler();

            //std::shared_ptr<Shader> shader_;
            std::shared_ptr<Shader> vs_{};
			ShaderReflectionInfo vsReflectionInfo_{};
            std::shared_ptr<Shader> ps_{};
			ShaderReflectionInfo psReflectionInfo_{};

            std::shared_ptr<RootSignature> rootSignature_{};
            std::shared_ptr<GraphicsPipelineState> pipelineState_{};
            std::shared_ptr<ym::Sampler> sampler_;

			bool isinitialized_ = false; // �������ς݂��ǂ���
            //ShaderReflectionInfo reflectionInfo_;

            std::unordered_map<std::string, MaterialConstantBufferInfo> cbuffers_; // �萔�o�b�t�@�Ǘ�
			std::unordered_map<std::string, MaterialTextureInfo> textures_; // �e�N�X�`���Ǘ�
            static Device *device_; // �f�o�C�X�ւ̃|�C���^
			static CommandList *cmdList_; // �R�}���h���X�g�ւ̃|�C���^
        };
	}
}