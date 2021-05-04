#pragma once

#include "AsyncLoader.hpp"
#include <vector>

namespace VSGE {
	class ResourceCache {
	private:
		AsyncLoader* loader;
		std::vector<Resource*> resources;

		void CreateResource(DataDescription& descr, ResourceType type);

	public:

		ResourceCache() {
			loader = new AsyncLoader;
			loader->Run();
		}

		~ResourceCache() {
			delete loader;
		}

		Resource* GetResource(const std::string& name);
		void AddResourceDir(const std::string& path);
		bool AddResourceBundle(const std::string& bundle_path);
	};
}