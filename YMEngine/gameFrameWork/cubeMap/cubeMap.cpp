#include "cubeMap.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"
#include "renderTexture/renderTexture.h"

namespace ym
{
	void CubeMapGenerater::Generate(Vector3 position)
	{
		Init();
		const float distance = 100.0f;
		transform.Position = position;

		for (int i = 0; i < (int)Direction::MAX; i++)
		{
			switch ((Direction)i)
			{
			case Direction::UP:
				transform.Rotation = { -90.0f,0.0f,0.0f };
				renderTexture[i]->SetCamera("cubeMapCameraUp");
				break;
			case Direction::DOWN:
				transform.Rotation = { 90.0f,0.0f,0.0f };
				renderTexture[i]->SetCamera("cubeMapCameraDown");
				break;
			case Direction::RIGHT:
				transform.Rotation = { 0.0f,90.0f,0.0f };
				renderTexture[i]->SetCamera("cubeMapCameraRight");
				break;
			case Direction::LEFT:
				transform.Rotation = { 0.0f,-90.0f,0.0f };
				renderTexture[i]->SetCamera("cubeMapCameraLeft");
				break;
			case Direction::FRONT:
				transform.Rotation = { 0.0f,0.0f,0.0f };
				renderTexture[i]->SetCamera("cubeMapCameraFront");
				break;
			case Direction::BACK:
				transform.Rotation = { 0.0f,180.0f,0.0f };
				renderTexture[i]->SetCamera("cubeMapCameraBack");
				break;
			default:
				break;	
			}
			camera[i]->SetTarget(transform.Position + transform.GetForward() * distance);
			camera[i]->SetUp(transform.GetUp());
			camera[i]->UpdateViewMatrix();
			camera[i]->UpdateProjectionMatrix();
			camera[i]->UpdateShaderBuffer();
			renderTexture[i]->Draw();
		}
		
			
		//for (int i = 0; i < (int)Direction::MAX; i++)
		//{
		//	switch ((Direction)i)	
		//	{
		//	case Direction::UP:
		//		transform.Rotation = { -90.0f,0.0f,0.0f };
	
		//		break;

		//	case Direction::DOWN:
		//		transform.Rotation = { 90.0f,0.0f,0.0f };
		//		break;

		//	case Direction::RIGHT:
		//		transform.Rotation = { 0.0f,90.0f,0.0f };
		//		break;
		//	case Direction::LEFT:
		//		transform.Rotation = { 0.0f,-90.0f,0.0f };
		//		break;

		//	case Direction::FRONT:
		//		transform.Rotation = { 0.0f,0.0f,0.0f };
		//		break;

		//	case Direction::BACK:
		//		transform.Rotation = { 0.0f,180.0f,0.0f };
		//		break;
		//	default:
		//		break;
		//	}
		//	transform.Rotation = {90.0f,0.0f,0.0f};
		//	camera->SetTarget(transform.Position + transform.GetForward() * distance);
		//	camera->SetUp(transform.GetUp());
		//	camera->UpdateViewMatrix();
		//	camera->UpdateProjectionMatrix();
		//	camera->UpdateShaderBuffer();
		//	renderTexture[i]->SetCamera("cubeMapCamera");
		//	renderTexture[i]->Draw();
		//	Vector3 disp = transform.Position + transform.GetForward() * distance;
		//	//ym::ConsoleLogRelease(disp.ToString().c_str());
		//	//ym::ConsoleLogRelease("\n");
		//	//camera->SetEye(transform.Position);
		//	//camera->Update();
		//	//renderTexture[i]->SetRenderTarget();
		//	//Renderer::Instance().Clear();
		//	//Renderer::Instance().Draw();
		//	//renderTexture[i]->ResetRenderTarget();
		//}

	}

	void CubeMapGenerater::Uninit()
	{
		if (!isInit)
		{
			return;
		}
		isInit = false;

		for (int i = 0; i < (int)Direction::MAX; i++)
		{
			renderTexture[i]->Uninit();
		}
	}

	void CubeMapGenerater::Init()
	{
		if (isInit)
		{
			return;
		}
		isInit = true;

		//カメラの生成
		camera[0] = CameraManager::Instance().CreateCamera("cubeMapCameraUp");
		camera[1] = CameraManager::Instance().CreateCamera("cubeMapCameraDown");

		camera[2] = CameraManager::Instance().CreateCamera("cubeMapCameraRight");
		camera[3] = CameraManager::Instance().CreateCamera("cubeMapCameraLeft");

		camera[4] = CameraManager::Instance().CreateCamera("cubeMapCameraFront");
		camera[5] = CameraManager::Instance().CreateCamera("cubeMapCameraBack");

		//レンダーテクスチャの生成
		for (int i = 0; i < (int)Direction::MAX; i++)
		{
			renderTexture[i] = std::make_shared<RenderTexture>();
			renderTexture[i]->Init();
			renderTexture[i]->Create(1024, 1024);
		}

	}
	void CubeMapGenerater::Draw()
	{
			
	}
}
