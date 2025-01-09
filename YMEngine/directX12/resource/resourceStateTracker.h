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
            return D3D12_RESOURCE_STATE_COMMON; // デフォルトの状態
        }

        void Uninit();

    private:
        std::unordered_map<ID3D12Resource *, D3D12_RESOURCE_STATES> resourceStates;
        static ResourceStateTracker *instance;
    };
	//ResourceStateTracker *ResourceStateTracker::instance = nullptr;
    //// 使用例
    //ResourceStateTracker tracker;
    //tracker.SetState(indexBufferResource, D3D12_RESOURCE_STATE_COPY_DEST);

    //// 状態遷移前に確認
    //D3D12_RESOURCE_STATES currentState = tracker.GetState(indexBufferResource);
    //if (currentState != D3D12_RESOURCE_STATE_INDEX_BUFFER) {
    //    // ResourceBarrierで状態を変更
    //}

}