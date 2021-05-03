#pragma once

#include "Resource.hpp"
#include <vector>

namespace VSGE {
	class ResourceCache {
	private:
		std::vector<Resource*> resources;
	public:

		ResourceCache() {
		}

		Resource* GetResource(const std::string& name);
		void AddResourceDir(const std::string& path);
		bool AddResourceBundle(const std::string& bundle_path);
	};
}