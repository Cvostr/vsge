#pragma once

#include <Core/VarTypes/Guid.hpp>
#include <string>

namespace VSGE {
	
	class ResourceCache;

	enum ResourceState {
		RESOURCE_STATE_UNLOADED = 0,
		RESOURCE_STATE_QUEUED,
		RESOURCE_STATE_LOADING,
		RESOURCE_STATE_LOADED,
		RESOURCE_STATE_READY,
		RESOURCE_STATE_LOADING_FAILED
	};

	enum ResourceType {
		RESOURCE_TYPE_NONE = 0,
		RESOURCE_TYPE_TEXTURE,
		RESOURCE_TYPE_MESHGROUP,
		RESOURCE_TYPE_MESH,
		RESOURCE_TYPE_AUDIOCLIP,
		RESOURCE_TYPE_ANIMATION,
		RESOURCE_TYPE_MATERIAL
	};

	struct DataDescription {
		std::string file_path;
		uint32 offset;
		uint32 size;
	};

	class Resource {
	protected:
		ResourceState mResourceState;
		uint32 mMemoryUse;
		std::string mName;

		byte* mLoadedData;
		DataDescription description;
	public:
		/// <summary>
		/// Get name of this resource
		/// </summary>
		/// <returns>resource name</returns>
		const std::string& GetName() { return mName; }
		/// <summary>
		/// Set name of this resource
		/// </summary>
		/// <param name="name">new resource name</param>
		void SetName(const std::string& name) { mName = name; }
		/// <summary>
		/// get current loading state of resource
		/// </summary>
		/// <returns>state of resource</returns>
		const ResourceState& GetState() { return mResourceState; }
		/// <summary>
		/// Set loading state of resource
		/// </summary>
		/// <param name="State"></param>
		void SetState(const ResourceState& State) { mResourceState = State; }
		/// <summary>
		/// Get size of memory, allocated for this resource
		/// </summary>
		/// <returns></returns>
		uint32 GetMemoryUse() { return mMemoryUse; }
		void SetMemoryUse(uint32 memory) { mMemoryUse = memory; }

		void SetDataDescription(const DataDescription& desc) {
			description = desc;
		}

		const DataDescription& GetDataDescription() {
			return description;
		}

		void SetLoadedData(byte* data) {
			mLoadedData = data;
		}

		void FreeLoadedData();

		byte* GetLoadedData() {
			return mLoadedData;
		}

		virtual void Load();
		virtual void Prepare() {}
		virtual void PostLoad() {}
		 
		//virtual ResourceType GetResourceType() = 0;

		Resource() :
			mResourceState(RESOURCE_STATE_UNLOADED),
			mMemoryUse(0),
			mLoadedData(nullptr)
		{}
	};

	template<class T>
	class ResourceReference {
	public:
		T* mResourcePointer;
		std::string mResourceName;

		void UpdateResourcePointer(const std::string& resourceName) {
			mResourceName = resourceName;
			mResourcePointer = (T*)ResourceCache::Get()->GetResource(mResourceName);
		}

		bool IsResourceSpecified() {
			return !mResourceName.empty();
		}

		ResourceReference() {
			mResourcePointer = nullptr;
		}
	};
}