#pragma once

#include "AsyncLoader.hpp"
#include <vector>

namespace VSGE {
	class ResourceCache {
	private:
		static ResourceCache* _this;
		
		AsyncLoader* loader;
		std::vector<Resource*> resources;

		void CreateResource(DataDescription& descr, ResourceType type);

	public:

		static ResourceCache* Get() {
			return _this;
		}

		AsyncLoader* GetAsyncLoader() {
			return loader;
		}

		ResourceCache() {
			_this = this;
			loader = new AsyncLoader;
			loader->Run();
		}

		~ResourceCache() {
			delete loader;
		}

		void PushResource(Resource* res);
		Resource* GetResource(const std::string& name);
		void AddResourceDir(const std::string& path);
		bool AddResourceBundle(const std::string& bundle_path);
	};
}