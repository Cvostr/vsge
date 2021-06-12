#pragma once

#include <Core/VarTypes/Guid.hpp>
#include <string>
#include <vector>

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
		ResourceState _resourceState;
		uint32 mMemoryUse;
		std::string _name;

		byte* _loadedData;
		DataDescription _description;

		Resource* _parent; //Resource, that created this resource
		std::vector<Resource*> _subresources; //Resources, that created from this resource
	public:
		/// <summary>
		/// Get name of this resource
		/// </summary>
		/// <returns>resource name</returns>
		const std::string& GetName() { return _name; }
		/// <summary>
		/// Set name of this resource
		/// </summary>
		/// <param name="name">new resource name</param>
		void SetName(const std::string& name) { _name = name; }
		/// <summary>
		/// get current loading state of resource
		/// </summary>
		/// <returns>state of resource</returns>
		const ResourceState& GetState() { return _resourceState; }
		/// <summary>
		/// Set loading state of resource
		/// </summary>
		/// <param name="State"></param>
		void SetState(const ResourceState& State) { _resourceState = State; }

		bool IsUnloaded() { return _resourceState == RESOURCE_STATE_UNLOADED; }

		bool IsReady() { return _resourceState == RESOURCE_STATE_READY; }
		/// <summary>
		/// Get size of memory, allocated for this resource
		/// </summary>
		/// <returns></returns>
		uint32 GetMemoryUse() { return mMemoryUse; }
		void SetMemoryUse(uint32 memory) { mMemoryUse = memory; }
		/// <summary>
		/// Get pointer to parent resource
		/// </summary>
		/// <returns></returns>
		Resource* GetParent();
		/// <summary>
		/// Set resource parent
		/// </summary>
		/// <param name="resource"></param>
		void SetParent(Resource* resource);

		void SetDataDescription(const DataDescription& desc);

		const DataDescription& GetDataDescription();

		void SetLoadedData(byte* data);

		void FreeLoadedData();

		byte* GetLoadedData();

		virtual void Load();
		virtual void Prepare() {}
		virtual void PostLoad() {}
		 
		virtual ResourceType GetResourceType() = 0;

		Resource() :
			_parent(nullptr),
			_resourceState(RESOURCE_STATE_UNLOADED),
			mMemoryUse(0),
			_loadedData(nullptr)
		{}
	};

	
	class ResourceReference {
		ResourceType _resourceType;
		Resource* _resourcePointer;
		std::string _resourceName;
		std::string _parentName;
	public:
		
		ResourceType GetResourceType() {
			return _resourceType;
		}

		void SetResourceType(ResourceType type) {
			_resourceType = type;
		}

		template<class T>
		T* GetResource() {
			return static_cast<T*>(_resourcePointer);
		}

		Resource* GetResource();

		const std::string& GetResourceName();

		void SetResource(const std::string& resourceName);

		void SetResource(Resource* resource);

		bool IsResourceSpecified();

		void operator=(ResourceReference& ref) {
			SetResource(ref._resourceName);
		}

		ResourceReference() {
			_resourcePointer = nullptr;
			_resourceType = RESOURCE_TYPE_NONE;
		}
	};
}