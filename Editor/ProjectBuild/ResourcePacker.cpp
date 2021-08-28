#include "ResourcePacker.hpp"
#include <fstream>
#include <Resources/ResourceCache.hpp>
#include <Core/FileLoader.hpp>

using namespace VSGEditor;
using namespace VSGE;

ResourcePacker::ResourcePacker(){
    _written_bytes = 0;
    _current_bundle = 0;
    _bundle_file_split_size = 500;
    _bundle_name_prefix = "bundle";
}

void ResourcePacker::SetOutputBundleDirectoryPath(const std::string& path){
    _output_dir = path;
    if (_output_dir[_output_dir.size() - 1] != '/')
        _output_dir.push_back('/');
}

void ResourcePacker::Write(){
    _written_bytes = 0;
    _current_bundle = 0;
    _map_entries.clear();

    ResourceCache* cache = ResourceCache::Get();
    
    for (uint32 resource_i = 0; resource_i < cache->GetResourcesCount(); resource_i++) {
        Resource* resource = cache->GetResources()[resource_i];

        if (resource->IsDefault())
            continue;

        byte* file_data = nullptr;
        uint32 file_size = 0;
        bool result = LoadFile(resource->GetDataDescription().file_path, (char**)&file_data, &file_size);

        if (result) {
            MapEntry entry;
            entry.resource_name = resource->GetName();
            entry.offset = _written_bytes;
            entry.size = file_size;
            WriteFileToBundle(file_data, file_size);

            entry.bundle_index = _current_bundle;
            _map_entries.push_back(entry);
        }
    }

    std::ofstream map_stream(_output_dir + "resources.map", std::ios::binary);
    char termi = '\0';
    for (uint32 bundle_i = 0; bundle_i < _current_bundle + 1; bundle_i++) {
        std::string bundle_name = _bundle_name_prefix + "." + std::to_string(bundle_i);

        map_stream.write(bundle_name.c_str(), bundle_name.size());
        map_stream.write(&termi, 1);
    }

    for (MapEntry& entry : _map_entries) {
        map_stream.write(entry.resource_name.c_str(), entry.resource_name.size());
        map_stream.write(&termi, 1);
        map_stream.write((char*)&entry.bundle_index, sizeof(uint32));
        map_stream.write((char*)&entry.offset, sizeof(uint32));
        map_stream.write((char*)&entry.size, sizeof(uint32));
    }
}

void ResourcePacker::WriteFileToBundle(byte* data, uint32 size) {
    if (_written_bytes >= _bundle_file_split_size) {
        _current_bundle++;
    }
    
    std::string output_file = _output_dir + _bundle_name_prefix + "." + std::to_string(_current_bundle);

    std::ofstream bundle_stream(output_file, std::ios::binary | std::ios::app);
    bundle_stream.write((char*)data, size);
    bundle_stream.close();
}