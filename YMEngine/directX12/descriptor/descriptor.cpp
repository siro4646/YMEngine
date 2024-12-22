
#include "descriptor.h"
#include "descriptorHeap/descriptorHeap.h"
#include "device/device.h"

namespace ym
{
	void Descriptor::Destroy()
	{
	}

	void Descriptor::Release()
	{
		if (pParentHeap_)
		{
			pParentHeap_->ReleaseDescriptor(this);
		}
	}

}

