#pragma once
namespace ym
{
    class BaseScene {

    public:
        virtual ~BaseScene() {};
        virtual void Init() = 0;
        virtual void UnInit() = 0;
		virtual void FixedUpdate() = 0;
        virtual void Update() = 0;
        virtual void Draw() = 0;
    };
} // namespace ym