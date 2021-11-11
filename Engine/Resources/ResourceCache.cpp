#include "ResourceCache.hpp"
#include <filesystem>
#include <fstream>
#include <Core/ByteSolver.hpp>

#include "ResourceTypes/TextureResource.hpp"
#include "ResourceTypes/MeshResource.hpp"
#include "ResourceTypes/MaterialResource.hpp"
#include "ResourceTypes/AnimationResource.hpp"
#include "ResourceTypes/AudioClipResource.hpp"
#include "ResourceTypes/SceneResource.hpp"
#include "ResourceTypes/PrefabResource.hpp"

namespace fs = std::filesystem;
using namespace VSGE;

//Create singleton
ResourceCache cache;
ResourceCache* ResourceCache::_this = nullptr;

ResourceCache::ResourceCache() {
    _this = this;
    _loader = new AsyncLoader;
    _loader->Run();
    _loader->SetThreadName("res_load_async");

    _watchdog = new ResourcesWatchdog;
    _watchdog->Run();
    _watchdog->SetThreadName("res_watchdog");
}

Resource* ResourceCache::GetResource(const std::string& name, ResourceType type) {
    if (type == RESOURCE_TYPE_NONE) {
        for (Resource* resource : _resources) {
            if (resource->GetName() == name)
                return resource;
        }
    }
    else {
        for (Resource* resource : _resources) {
            if (resource->GetName() == name && resource->GetResourceType() == type)
                return resource;
        }
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
    if (ext == ".png" || ext == ".dds" || ext == ".jpg" || ext == ".vstx")
        return RESOURCE_TYPE_TEXTURE;
    if (ext == ".vs3m")
        return RESOURCE_TYPE_MESHGROUP;
    if (ext == ".vsmt")
        return RESOURCE_TYPE_MATERIAL;
    if (ext == ".vsanim")
        return RESOURCE_TYPE_ANIMATION;
    if (ext == ".wav")
        return RESOURCE_TYPE_AUDIOCLIP;
    if (ext == ".scn")
        return RESOURCE_TYPE_SCENE;
    if (ext == ".prefab")
        return RESOURCE_TYPE_PREFAB;
    return RESOURCE_TYPE_NONE;
}

void ResourceCache::AddResourceFile(const std::string& path) {
    fs::directory_entry file(path);
    ResourceType type = GetTypeByFileExt(file.path().extension().string());
    if (type != RESOURCE_TYPE_NONE) {
        DataDescription ddescr = {};
        ddescr.file_path = path;
        ddescr.offset = 0;
        ddescr.size = 0;
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
    auto it = std::remove(_resources.begin(), _resources.end(), resource);
    _resources.pop_back();
    _watchdog->RemoveResource(resource);

    for (auto reference : _references) {
        reference->SetPointerToNull();
        reference->GetResource();
    }
}

Resource* ResourceCache::CreateResource(DataDescription& descr, ResourceType type) {
    Resource* res = nullptr;

    switch (type) {
        case RESOURCE_TYPE_TEXTURE :
            res = new TextureResource;
            break;
        case RESOURCE_TYPE_MESHGROUP :
            res = new MeshGroupResource;
            break;
        case RESOURCE_TYPE_MATERIAL :
            res = new MaterialResource;
            break;
        case RESOURCE_TYPE_ANIMATION:
            res = new AnimationResource;
            break;
        case (RESOURCE_TYPE_AUDIOCLIP) :
            res = new AudioClipResource;
            break;
        case (RESOURCE_TYPE_SCENE) :
            res = new SceneResource;
            break;
        case (RESOURCE_TYPE_PREFAB) :
            res = new PrefabResource;
            break;
        default:
            res = new PrefabResource;
            break;
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
        res->Load();
    }

    //Push resource
    PushResource(res);

    return res;
}

bool ResourceCache::AddResourceBundle(const std::string& bundle_map_path) {
    std::string directory = bundle_map_path;
    while ( *(directory.end() - 1) != '/')
        directory.pop_back();
    
    std::ifstream stream;
    stream.open(bundle_map_path, std::ios::binary | std::ios::ate);

    if (stream.fail()) {
        return false;
    }

    uint32 size = static_cast<uint32>(stream.tellg());
    stream.seekg(0, std::ios::beg);
    byte* data = new byte[size];
    stream.read((char*)data, size);
    stream.close();

    ByteSolver solver(data, size);

    std::vector<std::string> bundle_names;
    while (true) {
        std::string next = solver.ReadNextString();
        if (next == "__data__")
            break;
        else
            bundle_names.push_back(next);
    }
    uint64 count = solver.GetValue<uint64>();

    for (uint64 i = 0; i < count; i++) {
        std::string name = solver.ReadNextString();
        ResourceType type = solver.GetValue<ResourceType>();
        DataDescription descr;
        descr.file_path = directory + bundle_names[solver.GetValue<uint32>()];
        descr.offset = solver.GetValue<uint32>();
        descr.size = solver.GetValue<uint32>();
        Resource* res = CreateResource(descr, type);
        res->SetName(name);
    }

    return true;
}

void ResourceCache::PushResource(Resource* res) {
    _resources.push_back(res);
    _watchdog->AddResource(res);
}

void ResourceCache::AddResourceReference(ResourceReference* reference) {
    for (auto& reference_i : _references) {
        if (reference_i == reference)
            return;
    }

    _references.push_back(reference);
}
void ResourceCache::RemoveResourceReference(ResourceReference* reference) {
    bool exist = false;
    for (auto& reference_i : _references) {
        if (reference_i == reference)
            exist = true;
    }

    if (exist) {
        auto it = std::remove(_references.begin(), _references.end(), reference);
        _references.pop_back();
    }
}