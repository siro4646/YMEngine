#pragma once

#include"scene/baseScene.h"

namespace ym
{
#define TESTCULCNUM (10240) 
	class Renderer;
	class Object;

	class MainCamera;
	class UnorderedAccessView;
	class RootSignature;
	class Shader;
	class ComputePipelineState;
	class DescriptorSet;

	class Buffer;
	class BufferView;

	class TestScene2 : public BaseScene
	{
	public:
		TestScene2() {}
		~TestScene2()
		{
			UnInit();
		}

		void Init() override;
		void UnInit() override;
		void FixedUpdate() override;
		void Update() override;
		void Draw() override;

	private:
		void CreateAsset();

		void SendDataToGPU();

	Renderer *_renderer = nullptr;	
	std::shared_ptr<Object> _mainCamera;

	std::shared_ptr<RootSignature> _rootSignature;
	std::shared_ptr<Shader> _cs;
	std::shared_ptr<ComputePipelineState> _computePipelineState;

	// �f�B�X�N���v�^�Z�b�g
	std::shared_ptr<DescriptorSet> _descriptorSet;

	std::shared_ptr<Buffer> _inputBuffer;
	std::shared_ptr<BufferView> _inputBufferView;
	std::shared_ptr<Buffer> _outputBuffer;
	std::shared_ptr<UnorderedAccessView> _outputUAV;
	std::shared_ptr<Buffer> _readBackBuffer;

	struct TestData
	{
		float data;
		float padding[3]{};
	};
	
	// ���̓f�[�^
	std::vector<TestData>inputData;

	// �o�̓f�[�^�p
	float outputData[TESTCULCNUM];

	};
}