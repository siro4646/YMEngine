#pragma once

#include "bufferView/bufferView.h"

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

		// ビュー行列とプロジェクション行列の更新
		void UpdateViewMatrix();
		void UpdateProjectionMatrix(float aspectRatio, float fov, float nearZ, float farZ);
		void UpdateProjectionMatrix();
		void UpdateShaderBuffer();

		//カメラ情報の取得
		const DirectX::XMMATRIX &GetViewMatrix() const { return view_; }
		const DirectX::XMMATRIX &GetProjectionMatrix() const { return projection_; }
		const DirectX::XMMATRIX &GetViewProjectionMatrix() const { return view_ * projection_; }

		const D3D12_CPU_DESCRIPTOR_HANDLE &GetDescriptorHandle() const { return pConstBufferView_->GetDescInfo().cpuHandle; }

		//カメラの位置情報の取得
		const DirectX::XMFLOAT3 &GetTarget() const { return target_.ToXMFLOAT3(); }
		const DirectX::XMFLOAT3 &GetUp() const { return up_.ToXMFLOAT3(); }
		const DirectX::XMFLOAT3 &GetEye() const { return eye_.ToXMFLOAT3(); }

		void SetTarget(const Vector3 &target) { target_ = target; }
		void SetUp(const Vector3 &up) { up_ = up; }
		void SetEye(const Vector3 &eye) { eye_ = eye; }
		void SetDistance(float distance) { distance_ = distance; }

		//const DirectX::XMMATRIX &GetEye() const { return eye_; }

	private:

		bool CreateConstantBuffer();

		Device* pDevice_;
		CommandList* pCommandList_;

		Transform transform_;

		DirectX::XMMATRIX view_;
		DirectX::XMMATRIX projection_;

		//定数系
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
			DirectX::XMMATRIX view;//ビュー行列
			DirectX::XMMATRIX projection;//プロジェクション行列
			DirectX::XMFLOAT3 eye;//カメラの位置
		};
		CameraData *pMapppedData_{};

	};

}