#pragma once
#include "gameFrameWork/components/component.h"

#define REGISTER_COMPONENT(Type) \
    namespace { \
        struct Type##AutoRegister { \
            Type##AutoRegister() { \
                ym::ComponentRegistry::Register(#Type, [](ym::Object* owner) { \
                    return std::make_unique<Type>(owner); \
                }); \
            } \
        }; \
        static Type##AutoRegister _auto_register_##Type; \
    }

namespace ym
{

	class Component;
    using ComponentFactoryFunc = std::function<std::unique_ptr<Component>()>;

    class ComponentRegistry
    {
    public:
        using ComponentFactoryFunc = std::function<std::unique_ptr<Component>(Object *owner)>;

        // 登録
        static void Register(const std::string &name, ComponentFactoryFunc func)
        {
            GetMap()[name] = func;
        }

        // 生成
        static std::unique_ptr<Component> Create(const std::string &name, Object *owner)
        {
            auto &map = GetMap();
            auto it = map.find(name);
            if (it != map.end())
                return it->second(owner);
            return nullptr;
        }

        // 一覧取得（UIなどで表示したいとき用）
        static std::vector<std::string> GetRegisteredNames()
        {
            std::vector<std::string> names;
            for (const auto &[key, _] : GetMap())
                names.push_back(key);
            return names;
        }

    private:
        static std::unordered_map<std::string, ComponentFactoryFunc> &GetMap()
        {
            static std::unordered_map<std::string, ComponentFactoryFunc> map;
            return map;
        }
    };

}