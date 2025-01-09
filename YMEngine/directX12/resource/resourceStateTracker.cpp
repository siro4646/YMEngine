
#include "resourceStateTracker.h"

namespace ym {

	ResourceStateTracker *ResourceStateTracker::instance = nullptr;

	void ResourceStateTracker::Uninit()
	{
		resourceStates.clear();
		delete instance;
		instance = nullptr;
	}

}