#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>

namespace VSGEditor {
	class Project {
	private:
		std::string project_root_dir;
		std::string project_assets_dir;
		std::string project_cache_dir;
	public:
		Project(){}

		Project(
			const std::string& root, 
			const std::string& assets_dir, 
			const std::string& cache_dir);

		const std::string& GetRootDirectory() const;
		const std::string& GetAssetsDirectory() const;
		const std::string& GetCacheDirectory() const;
	};
}