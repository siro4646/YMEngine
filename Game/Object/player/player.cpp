#include "player.h"
#include "gameFrameWork/components/sprite/sprite.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "gameFrameWork/components/objLoader/objLoader.h"
#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"

#include "gameFrameWork/requiredObject/mainCamera.h"

#include "gameFrameWork/gameObject/gameObjectManager.h"

#include "gameFrameWork/material/pbrMaterial.h"

#include "renderTexture/renderTexture.h"

#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"

#include "gameFrameWork/components/rigidBody/rigidBody.h"

#include "gameFrameWork/collider/collider.h"

namespace ym
{
	void Player::Init()
	{
		name = "Player";
		//localTransform.Scale = { 3.0f,0.25f,3.0f };
		localTransform.Scale *= 1;
		//localTransform.Rotation = { 0.0f,0.0f,0.0f };
		//localTransform.Position = { 0.0f,-0.5f,1.2f };

		localTransform.Position = { 0.0f,1.0f,0.0f };

		const wchar_t *modelFile = L"asset/Alicia/FBX/Alicia_solid_Unity.FBX";

		//material->Init();

		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile,
			flag
		};

		renderTexture = std::make_shared<RenderTexture>();
		renderTexture->Init();
		renderTexture->Create(1280, 720,true);
		/*objLoader = AddComponent<OBJLoader>().get();
		meshes = objLoader->Load(importSetting);*/
		//fbxMaterial = std::make_shared<FBXMaterial>();

		//objLoader->SetMaterial(fbxMaterial);

		fbxLoader = AddComponent<FBXLoader>().get();
		meshes = fbxLoader->Load(importSetting);

		for (auto &mesh : meshes)
		{

			auto pbrMaterial = std::make_shared<PBRMaterial>();
			materials3.push_back(pbrMaterial);
		}
		int count = 0;
		for (auto &mesh : meshes)
		{
			fbxLoader->SetMaterial(materials3[count], count);
			count++;
		}		
		auto main = objectManager->FindGameObjects("MainCamera")[0];	
		main->localTransform.Position.y = 1;
		main->localTransform.Position.z = -2;

		AddChild(main);
		auto &cM = CameraManager::Instance();
		Transform cameraTransform;
		cameraTransform.Position = { 0.0f,10.0f,0.0f };
		//cameraTransform.Rotation = { 90.0f,0.0f,0.0f };
		cameraTransform.Scale = { 1.0f,1.0f,1.0f };

		 subCamera_ = cM.CreateCamera("testCamera");		
		 subCamera_->SetUp(cameraTransform.GetUp());
		 subCamera_->SetEye(cameraTransform.Position);
		 subCamera_->SetTarget(cameraTransform.Position + cameraTransform.GetForward()*100);
		 subCamera_->SetDistance(100);
		//localTransform.Position = { 0.0f,0.0f,0.0f };
		/*auto sprite = AddComponent<ym::Sprite>();
		sprite->LoadTexture("asset/texture/Jacket_BaseColor.jpg");*/

		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();

		sceneRenderRegistrar->AddRenderObject(this);

		auto boxCollider = AddComponent<BoxCollider>().get();
		//auto sphereCollider = AddComponent<SphereCollider>().get();

		rb = AddComponent<Rigidbody>().get();
		//rb->useGravity = false;

		//renderTexture->Draw();
	}

	void Player::FixedUpdate()
	{
		auto &input = KeyboardInput::Instance();


		const float speed = 10;
		if (input.GetKey("W"))
		{
			auto forward = localTransform.GetForward();
			rb->AddForce(forward * speed);
		}
		if (input.GetKey("S"))
		{
			auto forward = localTransform.GetForward();
			rb->AddForce(forward * -speed);

		}
		if (input.GetKey("A"))
		{
			auto right = localTransform.GetRight();

			rb->AddForce(right * -speed);

		}
		if (input.GetKey("D"))
		{
			auto right = localTransform.GetRight();

			rb->AddForce(right * speed);

		}


		Object::FixedUpdate();

		//localTransform.Position += localTransform.GetForward() * 0.05f;
	}

	void Player::Update()
	{
		auto &input = KeyboardInput::Instance();		

		subCamera_->UpdateViewMatrix();
		subCamera_->UpdateProjectionMatrix();
		subCamera_->UpdateShaderBuffer();

		if (!_parent)
		{
			
			{
				/*const float speed = 1;
				if (input.GetKey("W"))
				{
					auto forward = localTransform.GetForward();
					rb->AddForce(forward * speed);
				}
				if (input.GetKey("S"))
				{
					auto forward = localTransform.GetForward();
					rb->AddForce(forward * -speed);

				}
				if (input.GetKey("A"))
				{
					auto right = localTransform.GetRight();

					rb->AddForce(right * -speed);

				}
				if (input.GetKey("D"))
				{
					auto right = localTransform.GetRight();

					rb->AddForce(right * speed);

				}*/

			}
			{

				//if (input.GetKey("W"))
				//{
				//	auto forward = localTransform.GetForward();
				//	//rb->AddForce(forward * 4);
				//	localTransform.Position += 0.05f * forward;
				//}
				//if (input.GetKey("S"))
				//{
				//	//localTransform.Position.z -= 0.05f;
				//	auto forward = localTransform.GetForward();
				//	//rb->AddForce(forward * -4);
				//	localTransform.Position -= 0.05f * forward;

				//}
				//if (input.GetKey("A"))
				//{
				//	auto right = localTransform.GetRight();

				//	//rb->AddForce(right * -4);

				//	localTransform.Position -= 0.05f * right;
				//}
				//if (input.GetKey("D"))
				//{
				//	auto right = localTransform.GetRight();

				//	//rb->AddForce(right * 4);

				//	localTransform.Position += 0.05f * right;
				//}

			}


			if (input.GetKeyDown("0"))
			{
				localTransform.Position = { 0.0f,10.0f,2.0f };
				//localTransform.Position.y += 0.05f;
			}
			if (input.GetKeyDown("E"))
			{
				rb->AddForce(localTransform.GetUp() * 10,ForceMode::Impulse);
				//localTransform.Position.y -= 0.05f;
			}
			if (input.GetKeyDown("R"))
			{
				auto f = localTransform.GetForward();
				auto u = localTransform.GetUp();
				rb->AddForce(Vector3(f.x,0,f.z)*5, ForceMode::Impulse);
			}
		}

		if (input.GetKeyDown("LEFT"))
		{
			//localTransform.Rotation.y -= 1.0f;
			localTransform.Rotate({ 0.0f,-45.0f,0.0f });
			//localTransform.RotateEuler({ 0.0f,-40.0f,0.0f });
		}
		if (input.GetKeyDown("RIGHT"))
		{
			//localTransform.Rotation.y += 1.0f;
			localTransform.Rotate({ 0.0f,45.0f,0.0f });

			//localTransform.RotateEuler({ 0.0f,40.0f,0.0f });
		}		
		if (input.GetKeyDown("H"))
		{
			auto &cM = CameraManager::Instance();
			cM.SetMainCamera("testCamera");
		}
		if (input.GetKeyDown("J"))
		{
			auto &cM = CameraManager::Instance();
			cM.SetMainCamera("mainCamera");
		}
		Object::Update();
	}

	void Player::Draw()
	{
		//renderTexture->Draw();
		Object::Draw();
	}

	void Player::Uninit()
	{

		renderTexture->Uninit();	
		materials3.clear();
		meshes.clear();
		Object::Uninit();
	}

	std::shared_ptr<Object> Player::Clone()
	{
		auto copy = std::make_shared<Player>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}


}

