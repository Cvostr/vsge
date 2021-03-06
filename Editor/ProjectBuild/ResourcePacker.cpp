#include "ResourcePacker.hpp"
#include <Resources/ResourceCache.hpp>
#include <Core/FileLoader.hpp>
#include <MonoScripting/MonoScriptStorage.hpp>
#include <Core/ByteSerialize.hpp>
#include <Scene/SceneSerialization.hpp>
#include <Engine/Application.hpp>
#include <EditorLayers/EditorLayer.hpp>

using namespace VSGEditor;
using namespace VSGE;
using namespace YAML;

#define MEGS_2_BYTES 1024 * 1024

ResourcePacker::ResourcePacker(){
    _finished = true;
    _written_bytes = 0;
    _current_bundle = 0;
    _bundle_file_split_size = 100;
    _bundle_name_prefix = "bundle";

    SetThreadName("resource_packer");
}

void ResourcePacker::SetOutputBundleDirectoryPath(const std::string& path){
    _output_dir = path;
    if (_output_dir[_output_dir.size() - 1] != '/')
        _output_dir.push_back('/');
}

void ResourcePacker::THRFunc() {
    Write();
}

void ResourcePacker::Write(){
    _output_log.clear();
    _finished = false;
    _written_bytes = 0;
    _current_bundle = 0;
    _map_entries.clear();

    *this << "Compiling scripts\n";
    while (!MonoScriptStorage::Get()->IsScriptingReady());

    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();
    std::string project_cfg_path = el->GetProject().GetRootDirectory() + "/project.manifest";
    Node data = YAML::LoadFile(project_cfg_path);
    std::string project_name = data["project_name"].as<std::string>();
    std::string main_scene = data["main_scene"].as<std::string>();

    ResourceCache* cache = ResourceCache::Get();
    
    for (uint32 resource_i = 0; resource_i < cache->GetResourcesCount(); resource_i++) {
        Resource* resource = cache->GetResources()[resource_i];

        *this << "Writing resource " << resource->GetName() << "\n";

        if (resource->IsDefault())
            continue;

        byte* file_data = nullptr;
        uint32 file_size = 0;
        bool result = false;
        if(resource->GetResourceType() != RESOURCE_TYPE_SCENE)
            result = LoadFile(resource->GetDataDescription().file_path, (char**)&file_data, &file_size);
        else {
            VSGE::Scene* temp_scene = new Scene;
            temp_scene->NewScene();

            SceneSerializer serializer;
            serializer.SetScene(temp_scene);
            serializer.Deserialize(resource->GetDataDescription().file_path);
            serializer.SerializeBinary(&file_data, file_size);
            delete temp_scene;
            result = true;
        }

        if (result) {
            CheckForBundleOverflow();
            MapEntry entry;
            entry.resource_name = resource->GetName();
            entry.type = resource->GetResourceType();
            entry.offset = _written_bytes;
            entry.size = file_size;
            WriteFileToBundle(file_data, file_size);

            entry.bundle_index = _current_bundle - 1;
            _map_entries.push_back(entry);
        }

        SAFE_RELEASE_ARR(file_data);
    }
    _bundle_stream.close();
    //write scripts dll
    {
        byte* file_data = nullptr;
        uint32 file_size = 0;
        
        bool result = LoadFile(MonoScriptStorage::Get()->GetDllOutputPath(), (char**)&file_data, &file_size);
        if (result) {
            std::string output_file = _output_dir + "runtime.dll";
            std::ofstream scripts_stream(output_file, std::ios::binary);
            scripts_stream.write((char*)file_data, file_size);
            scripts_stream.close();
        }

        SAFE_RELEASE_ARR(file_data);

        result = LoadFile(MonoScriptStorage::Get()->GetApiDllPath(), (char**)&file_data, &file_size);
        if (result) {
            std::string output_file = _output_dir + "api.dll";
            std::ofstream scripts_stream(output_file, std::ios::binary);
            scripts_stream.write((char*)file_data, file_size);
            scripts_stream.close();
        }

        SAFE_RELEASE_ARR(file_data);
    }

    *this << "Writing map file\n";

    ByteSerialize serializer;
    std::ofstream map_stream(_output_dir + "resources.map", std::ios::binary);
    serializer.Serialize(std::string("__bundles__"));
    for (uint32 bundle_i = 0; bundle_i < _current_bundle; bundle_i++) {
        std::string bundle_name = _bundle_name_prefix + "." + std::to_string(bundle_i);
        serializer.Serialize(bundle_name);
    }
    //write __data__ header
    serializer.Serialize(std::string("__data__"));
    serializer.Serialize((uint64)_map_entries.size());
    for (MapEntry& entry : _map_entries) {
        serializer.Serialize(entry.resource_name);
        serializer.Serialize(entry.type);
        serializer.Serialize(entry.bundle_index);
        serializer.Serialize(entry.offset);
        serializer.Serialize(entry.size);
    }

    map_stream.write((char*)serializer.GetBytes(), serializer.GetSerializedSize());
    map_stream.close();

    //writing manifest
    std::ofstream manifest_stream(_output_dir + "app.manifest", std::ios::binary);
    serializer.Clear();
    serializer.Serialize(project_name);
    serializer.Serialize(main_scene);
    manifest_stream.write((char*)serializer.GetBytes(), serializer.GetSerializedSize());
    manifest_stream.close();

    *this << "Done\n";
    _finished = true;
}

bool ResourcePacker::IsFinished() {
    return _finished;
}

const std::string& ResourcePacker::GetOutput() {
    return _output_log;
}

void ResourcePacker::CheckForBundleOverflow() {
    if (_written_bytes >= _bundle_file_split_size * MEGS_2_BYTES || !_bundle_stream.is_open()) {
        //check, if stream opened, then close it
        if (_bundle_stream.is_open())
            _bundle_stream.close();

        std::string output_file = _output_dir + _bundle_name_prefix + "." + std::to_string(_current_bundle);
        _bundle_stream.open(output_file, std::ios::binary);
        _current_bundle++;
        _written_bytes = 0;
    }
}

void ResourcePacker::WriteFileToBundle(byte* data, uint32 size) {
    _bundle_stream.write((char*)data, size);
    _written_bytes += size;
}