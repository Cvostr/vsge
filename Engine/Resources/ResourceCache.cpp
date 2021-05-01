#include "ResourceCache.hpp"
#include <filesystem>

namespace fs = std::filesystem;

using namespace VSGE;

IResource* ResourceCache::GetResource(const std::string& name) {
    return nullptr;
}
void ResourceCache::AddResourceDir(const std::string& path) {
    for (const auto& entry : fs::directory_iterator(path)) {
        //if file starts with "." then don't show it
        if (entry.path().filename().string()[0] == '.')
            continue;
        if (entry.is_directory())
            AddResourceDir(entry.path().string());
        /*FileEntry e;
        e.isDir = entry.is_directory();
        e.abs_path = entry.path().string();
        e.name = entry.path().filename().string();
        e.ext = entry.path().extension().string();
        //Calculating relative path
        for (uint32 s = (uint32)mRootDir.size() + 1; s < (uint32)e.abs_path.size(); s++) {
            e.rel_path += e.abs_path[s];
        }
        //Push new entry to array
        mFiles.push_back(e);*/
    }
}
void ResourceCache::AddResourceBundle(ResourceBundle* bundle) {

}