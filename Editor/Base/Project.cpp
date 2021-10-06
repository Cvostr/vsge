#include "Project.hpp"

using namespace VSGEditor;

Project::Project(const std::string& root, const std::string& assets_dir, const std::string& cache_dir) {
	project_root_dir = root;
	for (uint32 i = 0; i < project_root_dir.size(); i++) {
		if (project_root_dir[i] == '\\')
			project_root_dir[i] = '/';
	}
	project_assets_dir = root + "/" + assets_dir;
	project_cache_dir = root + "/" + cache_dir;
}

const std::string& Project::GetRootDirectory() const {
	return project_root_dir;
}
const std::string& Project::GetAssetsDirectory() const {
	return project_assets_dir;
}
const std::string& Project::GetCacheDirectory() const {
	return project_cache_dir;
}