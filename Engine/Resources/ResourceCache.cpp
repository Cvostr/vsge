#include "ResourceCache.hpp"
#include <filesystem>
#include <fstream>
#include <Core/ByteSolver.hpp>

#include "ResourceTypes/TextureResource.hpp"
#include "ResourceTypes/MeshResource.hpp"
#include "ResourceTypes/MaterialResource.hpp"
#include "ResourceTypes/AnimationResource.hpp"
#include "ResourceTypes/AudioClipResource.hpp"

namespace fs = std::filesystem;
using namespace VSGE;

//Create singleton
ResourceCache cache;
ResourceCache* ResourceCache::_this = nullptr;

ResourceCache::ResourceCache() {
    _this = this;
    _loader = new AsyncLoader;
    _loader->Run();
    _loader->SetThreadName("Resource Async Loader");
}

Resource* ResourceCache::GetResource(const std::string& name) {
    for (Resource* resource : _resources) {
        if (resource->GetName() == name)
            return resource;
    }
    return nullptr;
}

Resource* ResourceCache::GetResourceWithFilePath(const std::string& fpath) {
    for (uint32 res_i = 0; res_i < GetResourcesCount(); res_i++) {
        VSGE::Resource* res = GetResources()[res_i];
        if (res->GetDataDescription().file_path == fpath)
            return res;
    }
    return nullptr;
}

ResourceType GetTypeByFileExt(const std::string& ext) {
    if (ext == ".png" || ext == ".dds")
        return RESOURCE_TYPE_TEXTURE;
    if (ext == ".vs3m")
        return RESOURCE_TYPE_MESHGROUP;
    if (ext == ".vsmt")
        return RESOURCE_TYPE_MATERIAL;
    if (ext == ".vsanim")
        return RESOURCE_TYPE_ANIMATION;
    if (ext == ".wav")
        return RESOURCE_TYPE_AUDIOCLIP;
    return RESOURCE_TYPE_NONE;
}

void ResourceCache::AddResourceFile(const std::string& path) {
    fs::directory_entry file(path);
    ResourceType type = GetTypeByFileExt(file.path().extension().string());
    if (type != RESOURCE_TYPE_NONE) {
        DataDescription ddescr = {};
        ddescr.file_path = path;
        ddescr.offset = 0;
        ddescr.size = static_cast<uint32>(file.file_size());
        CreateResource(ddescr, type);
    }
}

void ResourceCache::AddResourceDir(const std::string& path) {
    for (const auto& entry : fs::directory_iterator(path)) {
        //if file starts with "." then don't show it
        if (entry.path().filename().string()[0] == '.')
            continue;
        if (entry.is_directory())
            AddResourceDir(entry.path().string()); 
        //if it is a file
        if (!entry.is_directory()) {
            AddResourceFile(entry.path().string());
        }
    }
}

void ResourceCache::RemoveResource(Resource* resource) {
    std::remove(_resources.begin(), _resources.end(), resource);
}

void ResourceCache::CreateResource(DataDescription& descr, ResourceType type) {
    Resource* res = nullptr;

    if (type == RESOURCE_TYPE_TEXTURE) {
        res = new TextureResource;
    }
    else if (type == RESOURCE_TYPE_MESHGROUP) {
        res = new MeshGroupResource;
    }
    else if (type == RESOURCE_TYPE_MATERIAL) {
        res = new MaterialResource;
    }
    else if (type == RESOURCE_TYPE_ANIMATION) {
        res = new AnimationResource;
    }
    else if (type == RESOURCE_TYPE_AUDIOCLIP) {
        res = new AudioClipResource;
    }

    if(res != nullptr)
        res->SetDataDescription(descr);

    uint32 filename_pos = 0;
    uint32 ext_pos = 0;

    for (uint32 i = 0; i < descr.file_path.size(); i++) {
        char cur_char = descr.file_path[i];
        if (cur_char == '\\' || cur_char == '/')
            filename_pos = i;
        if (cur_char == '.')
            ext_pos = i;
    }

    std::string res_name = "";
    for (uint32 i = filename_pos + 1; i < ext_pos; i++) {
        char cur_char = descr.file_path[i];
        res_name.push_back(cur_char);
    }

    res->SetName(res_name);
    
    //if it is mesh group - load it to create meshes
    if (type == RESOURCE_TYPE_MESHGROUP) {
        _loader->AddToQueue(res);
    }

    _resources.push_back(res);
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
        ResourceType type = RESOURCE_TYPE_NONE;
        DataDescription ddescr = {};
        solver.Copy(&type);

    }

    return true;
}

void ResourceCache::PushResource(Resource* res) {
    _resources.push_back(res);
}