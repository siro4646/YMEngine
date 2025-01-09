
#include "resourceManager.h"

namespace ym
{
	ResourceManager *ResourceManager::instance=nullptr;

	void ResourceManager::Uninit()
	{
		//—v‘f‚ª‚ ‚é‚È‚ç
		ym::ConsoleLog("ResourceManager::Uninit()\n");
		ym::ConsoleLog("TextureMap Size : %d\n", textureMap_.size());

		if (!textureMap_.empty())
			textureMap_.clear();
		
		delete instance;
		instance = nullptr;
	}
}
