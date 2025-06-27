#pragma once

#include "../propertyProvider/propertyProvider.h"
#include "../propertyProvider/transform/transformPropertyProvider.h"

namespace ym {

    class GameObjectManager;
    class Component;

    class Object : public std::enable_shared_from_this<Object> {
    public:
        enum Type {
            Delete,
            None,
            Max,
        };

        GameObjectManager *objectManager = nullptr;

        Transform localTransform;
        Transform worldTransform;
        Type type = Type::None;
        std::vector<std::shared_ptr<Component>> components;
        std::vector<std::shared_ptr<Object>> _childs;
        std::string name;

        Object();
        virtual ~Object();

        virtual void Init();
        virtual void FixedUpdate();
        virtual void Update();
        virtual void Draw();
        virtual void Uninit();

        void DrawImgui();
        void DrawInspector();

        std::shared_ptr<Object> GetParent() const;
        void SetParent(std::shared_ptr<Object> newParent);

        virtual std::shared_ptr<Object> Clone();

        bool AddChild(std::shared_ptr<Object> child);
        bool DeleteChild(const std::shared_ptr<Object> &child);

        void UpdateHierarchy();

        template <typename T>
        std::shared_ptr<T> AddComponent() {
            auto component = std::make_shared<T>(this);
            component->Init();
            if (component->isFailed) {
                ym::ConsoleLog("Component %s initialization failed.\n", typeid(T).name());
                return nullptr;
            }
            components.push_back(component);
            return component;
        }

        void AddComponent(std::unique_ptr<Component> component);

        template <typename T>
        std::shared_ptr<T> GetComponent() {
            for (auto &component : components) {
                if (auto casted = std::dynamic_pointer_cast<T>(component)) {
                    return casted;
                }
            }
            //このオブジェクトとも探す
			for (auto &child : _childs) {
				if (auto found = child->GetComponent<T>()) {
					return found;
				}
			}
            return nullptr;
        }

        template <typename T>
        void RemoveComponent() {
            components.erase(
                std::remove_if(components.begin(), components.end(),
                    [](const std::shared_ptr<Component> &c) {
                        return std::dynamic_pointer_cast<T>(c) != nullptr;
                    }),
                components.end());
        }

        std::shared_ptr<Object> GetSharedPtr() {
            return shared_from_this();
        }
        std::vector<std::unique_ptr<IPropertyProvider>>CollectPropertyProviders();

    protected:
        std::shared_ptr<Object> _parent = nullptr;
        bool isActive = true;
        bool isUninit = false;
    };

} // namespace ym
