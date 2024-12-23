#pragma once

#include "device/device.h"
#include "commandList/commandList.h"


namespace ym
{
	class Device;
	class CommandList;

	//�`��֌W�S������Ă����N���X�ɂȂ�\��
	class Renderer
	{
	public:
		static Renderer *Instance()
		{
			static Renderer instance;
			return &instance;
		}
		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;



		bool Init(HWND hwnd, u32 width, u32 height, ColorSpaceType csType = ColorSpaceType::Rec709);

		void Uninit();

		void BeginFrame();
		void EndFrame();

	private:
		Renderer() = default;
		~Renderer()
		{

			Uninit();

		}

		void SetViewPort();

	private:
		std::shared_ptr<Device> device_{};//�f�o�C�X
		std::shared_ptr<CommandList> commandList_{};
		D3D12_VIEWPORT m_viewPort{};
		D3D12_RECT m_scissorRect{};
	};


}