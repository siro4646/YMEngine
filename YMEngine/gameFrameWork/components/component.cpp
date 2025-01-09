#include "component.h"

#include "gameFrameWork/gameObject/gameObject.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "Renderer/renderer.h"

namespace ym
{
	Component::Component(Object *object)
	{
		auto renderer = Renderer::Instance();
		pDevice_ = renderer->GetDevice()->shared_from_this();
		pCmdList_ = renderer->GetGraphicCommandList()->shared_from_this();
		this->object = object;
	}
}