#pragma once

#include "AsyncLoader.hpp"
#include <vector>

namespace VSGE {
	class ResourceCache {
	private:
		static ResourceCache* _this;
		
		AsyncLoader* _loader;
		std::vector<Resource*> _resources;

		void CreateResource(DataDescription& descr, ResourceType type);

	public:

		static ResourceCache* Get() {
			return _this;
		}

		AsyncLoader* GetAsyncLoader() {
			return _loader;
		}

		ResourceCache() {
			_this = this;
			_loader = new AsyncLoader;
			_loader->Run();
		}

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
		/// Get resource with specified name
		/// </summary>
		/// <param name="name">- name of resource to find</param>
		/// <returns>pointer to resource</returns>
		Resource* GetResource(const std::string& name);
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
	};
}