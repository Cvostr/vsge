#include "ResourcePacker.hpp"
#include <fstream>

using namespace VSGEditor;

ResourcePacker::ResourcePacker(){
    _written_bytes = 0;
}

void ResourcePacker::SetOutputBundleFilePath(const std::string& path){
    _output_file = path;
}

void ResourcePacker::Write(){

}