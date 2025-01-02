#pragma once



namespace ym
{
	class Object;
	class Device;
	class CommandList;

	class Component
	{
	protected:
		Object *object;
		std::shared_ptr<Device> pDevice_;
		std::shared_ptr<CommandList>pCmdList_;
	public:
		Component() = delete;
		Component(Object *object);
		virtual ~Component() {}
		virtual void Init() = 0;
		virtual void FixedUpdate() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;
		virtual void Uninit() = 0;
	};
}