#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>
#include <vector>
#include <mpi/Core/Guid.hpp>

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
		RESOURCE_TYPE_MATERIAL,
		RESOURCE_TYPE_SCRIPT,
		RESOURCE_TYPE_SCENE,
		RESOURCE_TYPE_PREFAB
	};

	struct DataDescription {
		std::string file_path;
		uint32 offset;
		uint32 size;

		DataDescription() :
			offset(0),
			size(0) {}
	};

#define RESOURCE_CLASS_TYPE(type) static ResourceType GetStaticType() { return type; } \
								virtual ResourceType GetResourceType() const { return GetStaticType(); }

	class Resource {
	protected:
		ResourceState m_resourceState;
		uint32 _memoryUse;
		uint64 _lastUseTime;
		std::string m_name;
		Guid m_id;

		byte* _loadedData;
		DataDescription _description;

		Resource* m_parent; //Resource, that created this resource
		std::vector<Resource*> _subresources; //Resources, that created from this resource
	public:

		Resource();
		virtual ~Resource();

		const Guid& getId() const;

		void SetId(const Guid& id);

		/// <summary>
		/// Get name of this resource
		/// </summary>
		/// <returns>resource name</returns>
		const std::string& GetName() const;
		/// <summary>
		/// Set name of this resource
		/// </summary>
		/// <param name="name">new resource name</param>
		void SetName(const std::string& name);
		/// <summary>
		/// get current loading state of resource
		/// </summary>
		/// <returns>state of resource</returns>
		const ResourceState& GetState() const;
		/// <summary>
		/// Set loading state of resource
		/// </summary>
		/// <param name="State"></param>
		void SetState(const ResourceState& State);
		/// <summary>
		/// Is resource fully unloaded
		/// </summary>
		/// <returns></returns>
		bool IsUnloaded() const;
		/// <summary>
		/// Is resource loaded and ready to use
		/// </summary>
		/// <returns></returns>
		bool IsReady() const;
		/// <summary>
		/// Get size of memory, allocated for this resource
		/// </summary>
		/// <returns></returns>
		uint32 GetMemoryUse() { return _memoryUse; }
		void SetMemoryUse(uint32 memory) { _memoryUse = memory; }
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
		/// <summary>
		/// Get array of child resource
		/// </summary>
		/// <returns></returns>
		std::vector<Resource*>& GetSubresources();

		Resource* GetSubresourceWithName(const std::string& name) const;
		/// <summary>
		/// Set data description
		/// </summary>
		/// <param name="desc"> - resource description</param>
		void SetDataDescription(const DataDescription& desc);
		/// <summary>
		/// Get data description
		/// </summary>
		/// <returns></returns>
		const DataDescription& GetDataDescription();
		/// <summary>
		/// Free memory of loaded data
		/// </summary>
		void FreeLoadedData();
		/// <summary>
		/// Get loaded data
		/// </summary>
		/// <returns></returns>
		byte* GetLoadedData();
		/// <summary>
		/// Get last frame number, where this resource was used
		/// </summary>
		/// <returns></returns>
		uint64 GetLastUseTime() const;
		/// <summary>
		/// Mark texture as used in this frame
		/// </summary>
		void Use();
		/// <summary>
		/// Release memory, occupied by resource in cpu or gpu
		/// </summary>
		void Release();

		void SetDefault();
		bool IsDefault() const;

		virtual void Load(bool just_read = false);
		void ReadData();
		/// <summary>
		/// Work to do on resource release
		/// </summary>
		virtual void OnRelease() {}
		virtual void Prepare() {}
		virtual void PostLoad() {}
		 
		virtual ResourceType GetResourceType() const = 0;
	};

}
