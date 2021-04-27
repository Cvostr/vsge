#pragma once

#include <Core/VarTypes/Guid.hpp>

namespace VSGE {
	
	enum ResourceState {
		RESOURCE_STATE_UNLOADED = 0,
		RESOURCE_STATE_QUEUED,
		RESOURCE_STATE_LOADING,
		RESOURCE_STATE_LOADED,
		RESOURCE_STATE_READY
	};
	
	class IResource {
	protected:
		ResourceState mState;
		Guid mId;
	public:

		const Guid& GetId() { return mId; }
		void SetId(Guid& id) { mId = id; }

		const ResourceState& GetState() { return mResourceState; }
		void SetState(const ResourceState& State) { mResourceState = State; }

		IResource() :
			mState(RESOURCE_STATE_UNLOADED)
		{}
	};
}