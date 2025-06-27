#pragma once
#include "../system/singleton/singleton.h"

namespace ym
{
		class FBXLoader;
		class OBJLoader;
		class PBRMaterial;

		class GameObjectManager;
		class Component;
		class Object;


	class MeshGenerator : public Singleton<MeshGenerator>
	{
	public:
		static Object *GenerateCube(const std::shared_ptr<GameObjectManager> &gameObjectManager, const std::string &name);
		static Object *GenerateSphere(const std::shared_ptr<GameObjectManager> &gameObjectManager, const std::string &name);
	};
}