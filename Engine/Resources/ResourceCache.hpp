#pragma once

#include "IResource.hpp"
#include "ResourceBundle.hpp"

namespace VSGE {
	class ResourceCache {
	private:
		std::vector<IResource*> resources;
	public:

		IResource* GetResource(const std::string& name);
		void AddResourceDir(const std::string& path);
		void AddResourceBundle(ResourceBundle* bundle);
	};
}