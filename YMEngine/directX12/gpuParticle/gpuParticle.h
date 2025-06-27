#pragma once

namespace ym
{
	class Renderer;
	class Device;
	class CommandList;

	class Shader;
	class RootSignature;
	class ComputePipelineState;
	class DescriptorSet;
	class Texture;
	class TextureView;

	class Buffer;
	class BufferView;
	class UnorderedAccessView;

#define MAX_PARTICLES 10000
#define PARTICLE_PER_THREAD 32

	struct ParticleHeader
	{
		INT32 tag;
		float depth;
	};

	class ParticleSystem
	{
	public:
		static ParticleSystem *Instance()
		{
			static ParticleSystem instance;
			return &instance;
		}

		ParticleSystem(){}

		~ParticleSystem(){}

		//�V�X�e���̏�����
		void Init();

		void Update();

		//�V�X�e���̉��
		void Uninit();

	private:

		//GPU����������
		void SystemInit();

		void CreateShader();
		void CreateRootSignature();
		void CreatePipelineState();

		void CreateSystemBuffer();

		void CreateParticleHeaderBuffer();


	private:
		Renderer *renderer_;
		Device *device_;
		CommandList *computeCommandList_;

		std::shared_ptr<Shader> cs_;
		std::shared_ptr<RootSignature> rootSignature_;
		std::shared_ptr<ComputePipelineState> sytemPipelineState_;
		std::shared_ptr<DescriptorSet> descriptorSet_;

		std::shared_ptr<Buffer> systemBuffer_;
		std::shared_ptr<UnorderedAccessView> systemBufferView_;
	
		std::shared_ptr<Buffer> phBuffer_;//�p�[�e�B�N���w�b�_�[
		std::shared_ptr<UnorderedAccessView> phBufferView_;

		std::shared_ptr<ComputePipelineState> phPipelineState_;


	};

}