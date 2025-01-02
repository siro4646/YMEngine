#pragma once

#include "device/device.h"
#include "commandList/commandList.h"


//テスト用
#include "test/polygon.h"


//=============================================================================


namespace ym
{
	class Device;
	class CommandList;
	class Window;

	//描画関係全部やってくれるクラスになる予定
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



		bool Init(Window *win, u32 width, u32 height, ColorSpaceType csType = ColorSpaceType::Rec709);

		void Uninit();

		void Update();

		void Resize(u32 width, u32 height);

		CommandList *GetCommandList()
		{
			return commandList_.get();
		}
		Device *GetDevice()
		{
			return device_.get();
		}


		void BeginFrame();
		void Draw();
		void EndFrame();

	private:
		Renderer() = default;
		~Renderer()
		{

			Uninit();

		}

		void SetViewPort();

		void CreateGraphicsItem();

	private:
		static std::shared_ptr<Device> device_;//デバイス
		static std::shared_ptr<CommandList> commandList_;
		ym::Window *window_{};
		D3D12_VIEWPORT m_viewPort{};
		D3D12_RECT m_scissorRect{};

		//テスト用
		ym::Polygon _polygon;

	};


}