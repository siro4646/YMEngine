#include "meshGenerator.h"
#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"
#include "gameFrameWork/gameObject/gameObject.h"

#include "gameFrameWork/components/fbxLoader/fbxLoader.h"
#include "gameFrameWork/components/objLoader/objLoader.h"
#include "gameFrameWork/material/pbrMaterial.h"

#include "gameFrameWork/collider/collider.h"

#include "gameFrameWork/components/mesh/filter/meshFilter.h"
#include "gameFrameWork/components/mesh/loader/meshLoader.h"
#include "gameFrameWork/components/mesh/renderer/meshRenderer.h"


namespace ym
{
	Object *MeshGenerator::GenerateCube(const std::shared_ptr<GameObjectManager> &gameObjectManager, const std::string &name)
	{
		Instance();
		auto cube = gameObjectManager->AddGameObject(std::make_shared<Object>());
		cube->name = name;

		mesh::MeshLoader::LoadMeshHierarchy(cube, "asset/model/cube.obj");

		auto col = cube->AddComponent<BoxCollider>().get();

		return cube.get();
	}
	Object *MeshGenerator::GenerateSphere(const std::shared_ptr<GameObjectManager> &gameObjectManager, const std::string &name)
	{
		Instance();
		auto sphere = gameObjectManager->AddGameObject(std::make_shared<Object>());
		sphere->name = name;

		mesh::MeshLoader::LoadMeshHierarchy(sphere, "asset/model/sphere_smooth.obj");

		auto col = sphere->AddComponent<SphereCollider>().get();

		return sphere.get();
	}
}