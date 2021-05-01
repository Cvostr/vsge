#pragma once

#include <Core/VarTypes/Guid.hpp>
#include <string>

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
		ResourceState mResourceState;
		Guid mId;
		uint32 mMemoryUse;
		std::string mName;
	public:

		const Guid& GetId() { return mId; }
		void SetId(Guid& id) { mId = id; }

		const std::string& GetName() { return mName; }
		void SetName(const std::string& name) { mName = name; }

		const ResourceState& GetState() { return mResourceState; }
		void SetState(const ResourceState& State) { mResourceState = State; }

		uint32 GetMemoryUse() { return mMemoryUse; }
		void SetMemoryUse(uint32 memory) { mMemoryUse = memory; }

		IResource() :
			mResourceState(RESOURCE_STATE_UNLOADED)
		{}
	};
}