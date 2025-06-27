#include "testScene2.h"
#include "renderer/renderer.h"

#include "Game/Object/TestObject.h"
#include "Game/Object/TestObject2.h"

#include "gameFrameWork/requiredObject/sphereMap/sphereMap.h"


#include "gameFrameWork/requiredObject/mainCamera.h"
#include "rootSignature/rootSignature.h"
#include "shader/shader.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/descriptorSet.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"
#include "descriptorSet/descriptorSet.h"

#include "Renderer/renderer.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "commandQueue/commandQueue.h"

#include "application/application.h"

#include "gpuParticle/gpuParticle.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "gameFrameWork/requiredObject/sceneOcTree/sceneOcTree.h"
#include "gameFrameWork/collider/collider.h"

#include "gameFrameWork/meshGenerator/meshGenerator.h"

#include "gameFrameWork/components/rigidBody/rigidBody.h"
#include "Game/Component/Test/testComponent.h"
#include "gameFrameWork/components/mesh/filter/meshFilter.h"
#include "gameFrameWork/components/mesh/loader/meshLoader.h"
#include "gameFrameWork/components/mesh/renderer/meshRenderer.h"

#include "components/animation/curve/animationCurve.h"
#include "components/animation/animator/animator.h"
#include "components/animation/clip/animationClip.h"

namespace ym
{
	// GameObjectにAnimatorを追加して、アニメーションクリップを設定する
	void SetupTestAnimation(Object *obj)
	{
		//// 1. Animatorを追加
		//auto animator = obj->AddComponent<ym::Animator>();
		//if (!animator) return;

		//// 2. クリップ作成
		//auto clip = std::make_shared<ym::AnimationClip>();
		//clip->length = 1.0f; // 1秒ループ

		//// 3. 対象の自分自身にカーブを設定（"ObjectName"）
		//// ※階層構造でRoot/Childなどがあればそのパスで指定
		//auto &curves = clip->AddCurvesForPath(obj->name);

		//// Y軸に沿って上下するアニメーション（0→1→0）
		//curves.posY.AddKey(0.0f, 0.0f);
		//curves.posY.AddKey(0.5f, 2.0f);
		//curves.posY.AddKey(1.0f, 0.0f);

		///*curves.sclX.AddKey(0.0f,obj->localTransform.Scale.x);
		//curves.sclX.AddKey(0.5f, 10.0f);
		//curves.sclX.AddKey(1.0f,obj->localTransform.Scale.x);

		//curves.sclY.AddKey(0.0f, obj->localTransform.Scale.y);
		//curves.sclY.AddKey(0.5f, 10.0f);
		//curves.sclY.AddKey(1.0f, obj->localTransform.Scale.y);

		//curves.sclZ.AddKey(0.0f, obj->localTransform.Scale.z);
		//curves.sclZ.AddKey(0.5f, 10.0f);
		//curves.sclZ.AddKey(1.0f, obj->localTransform.Scale.z);*/


		//// 4. Animatorにクリップ追加
		//animator->AddClip("Bounce", clip);

		//// 5. 再生（ループ）
		//animator->Play("Bounce", true);
	}


	void TestScene2::Init()
	{
		BaseScene::Init();

		ym::ConsoleLog("TestScene2::Init()\n");

		_renderer = Renderer::Instance();		

		gameObjectManager_->AddGameObject(std::make_shared<SphereMap>());
		_mainCamera = std::make_shared<MainCamera>();
		_mainCamera->localTransform.Position = Vector3(0, 2, -5);
		gameObjectManager_->AddGameObject(_mainCamera);

		gameObjectManager_->AddGameObject(std::make_shared<SceneOcTree>());

		auto obj = MeshGenerator::GenerateCube(gameObjectManager_, "Cube1");
		obj->localTransform.Scale.x *= 5;
		obj->localTransform.Scale.z *= 5;
		obj->localTransform.Rotation.y = 90;

		for (int i = 0; i < 5; i++)
		{
			auto obj2 = MeshGenerator::GenerateCube(gameObjectManager_, "Cube" + std::to_string(i));
			obj2->localTransform.Position.x += i * 2.0f;
			obj2->localTransform.Position.y += 1.0f;
			obj2->localTransform.Scale.x *= 0.5f;
			obj2->localTransform.Scale.z *= 0.5f;
		}

		//obj->AddComponent<Animator>();
		//SetupTestAnimation(obj);
		/*auto obj2 = MeshGenerator::GenerateCube(gameObjectManager_, "Cube2");
		obj2->localTransform.Position.y += 2.0f;
		obj2->localTransform.Scale.x *= 4;
		obj2->localTransform.Scale.z *= 4;
		obj2->AddComponent<Rigidbody>().get();*/

		/*auto sphere = MeshGenerator::GenerateSphere(gameObjectManager_, "Sphere");
		sphere->localTransform.Position.y += 2.0f;
		sphere->localTransform.Scale.x *= 2;
		sphere->localTransform.Scale.y *= 2;
		sphere->localTransform.Scale.z *= 2;
		sphere->AddComponent<Rigidbody>();*/
		//mf->SetMeshInstance(mesh::MeshLoader::LoadMesh("asset/Alicia/FBX/Alicia_solid_Unity.FBX"));
		//mf->SetMeshInstance(mesh::MeshLoader::LoadMesh("asset/sponza (1)/sponza.obj"));
		//sphere->AddComponent<TestComponent>();

		/*sphere = MeshGenerator::GenerateSphere(gameObjectManager_, "Sphere2");
		sphere->localTransform.Position.y += 2.0f;
		sphere->localTransform.Position.x += 2.0f;
		sphere->localTransform.Scale.x *= 2;
		sphere->localTransform.Scale.y *= 2;
		sphere->localTransform.Scale.z *= 2;
		sphere->AddComponent<Rigidbody>();*/
		//sphere->AddComponent<TestComponent>();



		/*auto obj1 = gameObjectManager_->AddGameObject(std::make_shared<Object>());
		obj1->name = "TestObject";
		obj1->localTransform.Position.x = 0;
		obj1->localTransform.Position.y = 1;*/
		//SetupTestAnimation(obj1.get());
		//mesh::MeshLoader::LoadMeshHierarchy(obj1, "asset/Alicia/FBX/Alicia_solid_Unity.FBX");
		//mesh::MeshLoader::LoadMeshHierarchy(obj1, "asset/X Bot.fbx");
		//mesh::MeshLoader::LoadMeshHierarchy(obj1, "asset/sponza1/sponza.obj");
		//mesh::MeshLoader::LoadMeshHierarchy(obj1, "asset/model/cube.obj");
		//auto mf = obj1->AddComponent<mesh::MeshFilter>().get();
		//mf->SetMeshInstance(mesh::MeshLoader::LoadMesh("asset/Alicia/FBX/Alicia_solid_Unity.FBX"));
		//obj1->AddComponent(ComponentRegistry::Create("MeshRenderer", obj1.get()));
		//auto mr = obj1->AddComponent<mesh::MeshRenderer>().get();

		//obj->localTransform.Scale.x *= 5;
		//obj->localTransform.Scale.z *= 5;
		//obj->localTransform.Rotation.y = 90;
		////auto fbx = obj->AddComponent<FBXLoader>().get();
		////fbx->Load(ImportSettings{ L"asset/model/cube.obj", ModelSetting::InverseV | ModelSetting::AdjustCenter | ModelSetting::AdjustScale });
		//auto col = obj->AddComponent<BoxCollider>().get();



		//_mainCamera->Init();
	}
	void TestScene2::UnInit()
	{
		BaseScene::UnInit();
		ym::ConsoleLog("TestScene2::UnInit()\n");

		_rootSignature.reset();
		_cs.reset();
		_computePipelineState.reset();
		_descriptorSet.reset();

		_inputBuffer.reset();
		_inputBufferView.reset();
		_outputBuffer.reset();
		_outputUAV.reset();
		_readBackBuffer.reset();





	}
	void TestScene2::FixedUpdate()
	{
		BaseScene::FixedUpdate();
	}
	void TestScene2::Update()
	{
		_renderer->Update();



		BaseScene::Update();
	}
	void TestScene2::Draw()
	{
		_renderer->BeginFrame();
		BaseScene::Draw();
		//SendDataToGPU();
		_renderer->Draw();
		_renderer->EndFrame();
	}
	void TestScene2::CreateAsset()
	{
		auto device = Renderer::Instance()->GetDevice();
		_cs = std::make_shared<Shader>();
		_cs->Init(device, ShaderType::Compute, "cs");
		_rootSignature = std::make_shared<RootSignature>();
		_rootSignature->Init(device, _cs.get());
		_computePipelineState = std::make_shared<ComputePipelineState>();
		ComputePipelineStateDesc desc;
		desc.pRootSignature = _rootSignature.get();
		desc.pCS = _cs.get();
		_computePipelineState->Init(device, desc);

		_descriptorSet = std::make_shared<DescriptorSet>();



		// 入力データ
		inputData.resize(TESTCULCNUM);
		for (int i = 0; i < TESTCULCNUM; ++i)
		{
			inputData[i].data = i;
		}
		//outputData.resize(1024);

		size_t dataSize = inputData.size() * sizeof(TestData);
		_inputBuffer = std::make_shared<Buffer>();
		_inputBuffer->Init(device,dataSize, sizeof(TestData),BufferUsage::ShaderResource,false,true);
		_inputBufferView = std::make_shared<BufferView>();
		_inputBufferView->Init(device, _inputBuffer.get(), 0, inputData.size(), sizeof(TestData));
		_inputBuffer->UpdateBuffer(device, _renderer->GetGraphicCommandList(),inputData.data(), dataSize);

		_outputBuffer = std::make_shared<Buffer>();
		_outputBuffer->Init(device,dataSize, sizeof(float), BufferUsage::ShaderResource, false, true);
		_outputUAV = std::make_shared<UnorderedAccessView>();
		_outputUAV->Initialize(device, _outputBuffer.get(), 0, inputData.size(), sizeof(float), 0);

		_readBackBuffer = std::make_shared<Buffer>();
		_readBackBuffer->Init(device, dataSize, sizeof(float), BufferUsage::ReadBack, false, false);
		
	}
	void TestScene2::SendDataToGPU()
	{
		auto ds = _descriptorSet.get();
		auto comCmdList = _renderer->GetComputeCommandList();

		comCmdList->Reset();
		ds->Reset();

		// 入力バッファの状態を設定
		comCmdList->TransitionBarrier(_inputBuffer.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		// 出力バッファの状態を設定
		comCmdList->TransitionBarrier(_outputBuffer.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// デスクリプタセットを設定
		ds->SetCsSrv(0, _inputBufferView->GetDescInfo().cpuHandle);
		ds->SetCsUav(0, _outputUAV->GetDescInfo().cpuHandle);

		comCmdList->GetCommandList()->SetPipelineState(_computePipelineState->GetPSO());
		comCmdList->SetComputeRootSignatureAndDescriptorSet(_rootSignature.get(), ds);

		// Compute Shader の実行
		comCmdList->GetCommandList()->Dispatch(TESTCULCNUM / 256, 1, 1);

		// UAV 書き込みの完了を保証
		comCmdList->UAVBarrier(_outputBuffer.get());

		// 出力バッファを COPY_SOURCE に遷移
		comCmdList->TransitionBarrier(_outputBuffer.get(), D3D12_RESOURCE_STATE_COPY_SOURCE);

		// 出力バッファの内容を ReadBack バッファにコピー
		comCmdList->GetCommandList()->CopyResource(
			_readBackBuffer->GetResourceDep(),
			_outputBuffer->GetResourceDep()
		);

		comCmdList->Close();
		comCmdList->Execute();

		_renderer->GetDevice()->WaitForComputeCommandQueue();

		// ReadBack バッファからデータを取得
		float *resultData = reinterpret_cast<float *>(_readBackBuffer->Map());
		/*for (int i = 0; i < TESTCULCNUM; ++i) {
			std::cout << "Result[" << i << "] = " << resultData[i] << std::endl;
		}*/
		_readBackBuffer->Unmap();
	}

}
