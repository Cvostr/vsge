#pragma once

#include "AsyncLoader.hpp"
#include "ResourcesWatchdog.hpp"
#include <vector>
#include <typeinfo>

namespace VSGE {

	class ResourceReference;

	class ResourceCache {
	private:
		static ResourceCache* _this;
		
		AsyncLoader* _loader;
		ResourcesWatchdog* _watchdog;

		std::vector<Resource*> _resources;
		std::vector<ResourceReference*> _references;

		void CreateResource(DataDescription& descr, ResourceType type);

	public:

		static ResourceCache* Get() {
			return _this;
		}

		AsyncLoader* GetAsyncLoader() {
			return _loader;
		}

		ResourceCache();

		~ResourceCache() {
			delete _loader;
		}
		/// <summary>
		/// Get count of currently cached resources
		/// </summary>
		/// <returns></returns>
		uint32 GetResourcesCount() {
			return static_cast<uint32>(_resources.size());
		}
		/// <summary>
		/// Get list of currently cached resources
		/// </summary>
		/// <returns></returns>
		Resource** GetResources() {
			return _resources.data();
		}
		/// <summary>
		/// Add resource pointer to resources vector
		/// </summary>
		/// <param name="res"></param>
		void PushResource(Resource* res);
		/// <summary>
		/// Remove resource from cache list
		/// </summary>
		/// <param name="resource">- resource to remove</param>
		void RemoveResource(Resource* resource);
		/// <summary>
		/// Get resource with specified name
		/// </summary>
		/// <param name="name">- name of resource to find</param>
		/// <returns>pointer to resource</returns>
		Resource* GetResource(const std::string& name, ResourceType type = RESOURCE_TYPE_NONE);

		template<typename T>
		T* GetResource(const std::string& name) {
			for (Resource* resource : _resources) {
				if (resource->GetName() == name && typeid(*resource) == typeid(T))
					return (T*)resource;
			}
			return nullptr;
		}

		Resource* GetResourceWithFilePath(const std::string& fpath);
		/// <summary>
		/// Add resource from specified file on disk
		/// </summary>
		/// <param name="path"></param>
		void AddResourceFile(const std::string& path);
		/// <summary>
		/// Add all resource in specified directory and its subdirectories
		/// </summary>
		/// <param name="path">- path to directory on disk</param>
		void AddResourceDir(const std::string& path);
		/// <summary>
		/// Add all resources from bundle file
		/// </summary>
		/// <param name="bundle_path">- path to resource bundle file on disk</param>
		/// <returns></returns>
		bool AddResourceBundle(const std::string& bundle_path);

		void AddResourceReference(ResourceReference* reference);
		void RemoveResourceReference(ResourceReference* reference);
	};
}