#pragma once

#include "bufferView/bufferView.h"

#include "gameFrameWork/raycast/raycast.h"
#include "winAPI/window/window.h"

namespace ym
{
	class Device;
	class CommandList;

	class Buffer;
	class ConstantBufferView;

	class DescriptorSet;

	class Camera
	{
	public:
		Camera();
		~Camera();

		// �r���[�s��ƃv���W�F�N�V�����s��̍X�V
		void UpdateViewMatrix();
		void UpdateProjectionMatrix(float aspectRatio, float fov, float nearZ, float farZ);
		void UpdateProjectionMatrix();
		void UpdateShaderBuffer();

		//�J�������̎擾
		const DirectX::XMMATRIX &GetViewMatrix() const { return view_; }
		const DirectX::XMMATRIX &GetProjectionMatrix() const { return projection_; }
		const DirectX::XMMATRIX &GetViewProjectionMatrix() const { return view_ * projection_; }

		const DirectX::XMMATRIX &GetInvViewMatrix() const { return DirectX::XMMatrixInverse(nullptr, view_); }
		const DirectX::XMMATRIX &GetInvProjectionMatrix() const { return DirectX::XMMatrixInverse(nullptr, projection_); }
		const DirectX::XMMATRIX &GetInvViewProjectionMatrix() const { return DirectX::XMMatrixInverse(nullptr, GetViewProjectionMatrix()); }


		const D3D12_CPU_DESCRIPTOR_HANDLE &GetDescriptorHandle() const { return pConstBufferView_->GetDescInfo().cpuHandle; }

		//�J�����̈ʒu���̎擾
		const DirectX::XMFLOAT3 &GetTarget() const { return target_.ToXMFLOAT3(); }
		const DirectX::XMFLOAT3 &GetUp() const { return up_.ToXMFLOAT3(); }
		const DirectX::XMFLOAT3 &GetEye() const { return eye_.ToXMFLOAT3(); }
		const float GetNearZ() const { return nearZ_; }
		const float GetFarZ() const { return farZ_; }
		const float GetAspectRatio() const { return aspectRatio_; }
		const float GetFov() const { return fov_; }

		void SetTarget(const Vector3 &target) { target_ = target; }
		void SetUp(const Vector3 &up) { up_ = up; }
		void SetEye(const Vector3 &eye) { eye_ = eye; }
		void SetDistance(float distance) { 
			distance_ = distance; 
			farZ_ = distance_;
		}
		
		std::string name_;

		Ray ScreenPointToRay()
		{
			auto mousePos = MouseInput::Instance().GetMousePos();
			float ndcX = (2.0f * mousePos.x / Application::Instance()->GetWindow()->GetWndWidth()) - 1.0f;
			float ndcY = 1.0f - (2.0f * mousePos.y / Application::Instance()->GetWindow()->GetWndHeight());

			Vector3 nearPos = Unproject(ndcX, ndcY, 0);
			Vector3 farPos = Unproject(ndcX, ndcY, 1);
			Vector3 direction = (farPos - nearPos).Normalize();
			return Ray(nearPos, direction);
		}
		

		//const DirectX::XMMATRIX &GetEye() const { return eye_; }

	private:

		Vector3 Unproject(float ndcX, float ndcY, float ndcZ)
		{
			DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(nullptr, GetViewProjectionMatrix());

			// NDC���W�x�N�g��
			DirectX::XMVECTOR ndcPos = DirectX::XMVectorSet(ndcX, ndcY, ndcZ, 1.0f);

			// ������ w���Z�t���Ń��[���h��Ԃɖ߂�
			DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(ndcPos, invViewProj);

			return Vector3(
				DirectX::XMVectorGetX(worldPos),
				DirectX::XMVectorGetY(worldPos),
				DirectX::XMVectorGetZ(worldPos)
			);
		}


		bool CreateConstantBuffer();

		Device* pDevice_;
		CommandList* pCommandList_;

		Transform transform_;

		DirectX::XMMATRIX view_;
		DirectX::XMMATRIX projection_;

		//�萔�n
		std::shared_ptr<Buffer> pConstantBuffer_;
		std::shared_ptr<ConstantBufferView>pConstBufferView_;
		std::shared_ptr<DescriptorSet> pDescriptorSet_;

		Vector3 target_;
		Vector3 up_;
		Vector3 eye_;

		float distance_ = 1000.0f;

		float aspectRatio_ = 1.0f;
		float fov_ = 60.0f;
		float nearZ_ = 0.1f;
		float farZ_ = 1000.0f;

		struct CameraData
		{
			DirectX::XMMATRIX view{};//�r���[�s��
			DirectX::XMMATRIX projection{};//�v���W�F�N�V�����s��
			//==================================================
			DirectX::XMMATRIX invView{};
			DirectX::XMMATRIX invProjection{};
			DirectX::XMMATRIX invViewProj{};
			//==================================================
			DirectX::XMFLOAT4 eye{};//�J�����̈ʒu
			float nearZ{};
			float farZ{};
			float aspect{};
			float fov{};
			float padding[40]{};
		};







		CameraData *pMapppedData_{};

	};

}