#include "camera.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "swapChain/swapChain.h"

#include "buffer/buffer.h"

#include "descriptorSet/DescriptorSet.h"

#include "Renderer/renderer.h"


namespace ym 
{
	Camera::Camera()
	{
		auto _renderer = Renderer::Instance();
		if (!_renderer)
		{
			return;
		}
		pDevice_ = _renderer->GetDevice();
		pCommandList_ = _renderer->GetGraphicCommandList();
		auto &swapChain = pDevice_->GetSwapChain();
		Vector2 windowSize = swapChain.GetSize();

		eye_ = Vector3(0.0f, 0.0f, -1.0f);
		target_ = Vector3(eye_.x,eye_.y,distance_ - eye_.z);
		up_ = Vector3(0.0f, 1.0f, 0.0f);

		aspectRatio_ = windowSize.x / windowSize.y;
		fov_ = 60.0f;
		nearZ_ = 0.1f;
		farZ_ = 1000.0f;

		CreateConstantBuffer();
		UpdateViewMatrix();
		UpdateProjectionMatrix(aspectRatio_,fov_,nearZ_,farZ_);
		UpdateShaderBuffer();
	}
	Camera::~Camera()
	{
		if (pConstantBuffer_)
		{
			pConstantBuffer_->Unmap();
			pConstantBuffer_.reset();
		}
		if (pConstBufferView_)
		{
			pConstBufferView_->Destroy();
			pConstBufferView_.reset();
		}


	}
	void Camera::UpdateViewMatrix()
	{
		//eye_.x += 0.01f;
		auto eye = eye_.ToXMFLOAT3();
		DirectX::XMVECTOR eyeVector = DirectX::XMLoadFloat3(&eye);
		auto target = target_.ToXMFLOAT3();
		DirectX::XMVECTOR targetVector = DirectX::XMLoadFloat3(&target);
		auto up = up_.ToXMFLOAT3();
		DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&up);

		view_ = DirectX::XMMatrixLookAtLH(eyeVector, targetVector, upVector);
		//transpose
		view_ = DirectX::XMMatrixTranspose(view_);


	}
	void Camera::UpdateProjectionMatrix(float aspectRatio, float fov, float nearZ, float farZ)
	{
		projection_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, nearZ, farZ);
		//transpose
		projection_ = DirectX::XMMatrixTranspose(projection_);

	}
	void Camera::UpdateProjectionMatrix()
	{
		projection_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov_), aspectRatio_, nearZ_, farZ_);
		//transpose
		projection_ = DirectX::XMMatrixTranspose(projection_);
	}
	void Camera::UpdateShaderBuffer()
	{
		pMapppedData_->view = DirectX::XMMatrixTranspose(view_);
		pMapppedData_->projection = DirectX::XMMatrixTranspose(projection_);
		pMapppedData_->eye = eye_.ToXMFLOAT3();
	}

	bool Camera::CreateConstantBuffer()
	{
		pConstantBuffer_ = std::make_shared<Buffer>();
		if (!pConstantBuffer_->Init(pDevice_, sizeof(pMapppedData_), sizeof(CameraData), BufferUsage::ConstantBuffer, true, false))
		{
			return false;
		}

		pConstBufferView_ = std::make_shared<ConstantBufferView>();

		if (!pConstBufferView_->Init(pDevice_, pConstantBuffer_.get()))
		{
			return false;
		}
		pMapppedData_ = (Camera::CameraData *)pConstantBuffer_->Map();
		if (!pMapppedData_)
		{
			return false;
		}

		pDescriptorSet_ = std::make_shared<DescriptorSet>();



		return false;

	}
}
