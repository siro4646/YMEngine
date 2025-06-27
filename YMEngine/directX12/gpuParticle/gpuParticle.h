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

		//システムの初期化
		void Init();

		void Update();

		//システムの解放
		void Uninit();

	private:

		//GPU側を初期化
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
	
		std::shared_ptr<Buffer> phBuffer_;//パーティクルヘッダー
		std::shared_ptr<UnorderedAccessView> phBufferView_;

		std::shared_ptr<ComputePipelineState> phPipelineState_;


	};

}