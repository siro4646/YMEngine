#pragma once
namespace ym
{
    class ResourceStateTracker {
    public:
		static ResourceStateTracker *Instance()
		{
			//=nullptr;
            if (!instance)
                instance = new ResourceStateTracker();

			return instance;
		}

        void SetState(ID3D12Resource *resource, D3D12_RESOURCE_STATES state) {
            resourceStates[resource] = state;
        }

        D3D12_RESOURCE_STATES GetState(ID3D12Resource *resource) const {
            auto it = resourceStates.find(resource);
            if (it != resourceStates.end()) {
                return it->second;
            }
            return D3D12_RESOURCE_STATE_COMMON; // �f�t�H���g�̏��
        }

        void Uninit();

    private:
        std::unordered_map<ID3D12Resource *, D3D12_RESOURCE_STATES> resourceStates;
        static ResourceStateTracker *instance;
    };
	//ResourceStateTracker *ResourceStateTracker::instance = nullptr;
    //// �g�p��
    //ResourceStateTracker tracker;
    //tracker.SetState(indexBufferResource, D3D12_RESOURCE_STATE_COPY_DEST);

    //// ��ԑJ�ڑO�Ɋm�F
    //D3D12_RESOURCE_STATES currentState = tracker.GetState(indexBufferResource);
    //if (currentState != D3D12_RESOURCE_STATE_INDEX_BUFFER) {
    //    // ResourceBarrier�ŏ�Ԃ�ύX
    //}

}