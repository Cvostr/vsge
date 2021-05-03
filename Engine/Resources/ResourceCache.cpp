#include "ResourceCache.hpp"
#include <filesystem>
#include <fstream>
#include <Core/ByteSolver.hpp>

namespace fs = std::filesystem;
using namespace VSGE;

Resource* ResourceCache::GetResource(const std::string& name) {
    for (Resource* resource : resources) {
        if (resource->GetName() == name)
            return resource;
    }
    return nullptr;
}

ResourceType GetTypeByFileExt(const std::string& ext) {
    if (ext == ".png" || ext == ".dds")
        return RESOURCE_TYPE_TEXTURE;
    return RESOURCE_TYPE_NONE;
}

void ResourceCache::AddResourceDir(const std::string& path) {
    for (const auto& entry : fs::directory_iterator(path)) {
        //if file starts with "." then don't show it
        if (entry.path().filename().string()[0] == '.')
            continue;
        if (entry.is_directory())
            AddResourceDir(entry.path().string()); 
        /*FileEntry e;
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
bool ResourceCache::AddResourceBundle(const std::string& bundle_path) {
    std::ifstream stream;
    stream.open(bundle_path, std::ios::binary | std::ios::ate);

    if (stream.fail()) {
        return false;
    }

    uint32 size = static_cast<uint32>(stream.tellg());
    stream.seekg(0, std::ios::beg);
    byte* data = new byte[size];
    stream.read((char*)data, size);
    stream.close();

    uint32 resources_count = 0;
    ByteSolver solver(data, size);
    solver.Copy(resources_count);
    while (!solver.end()) {

    }

    return true;
}