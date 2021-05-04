#include "ResourceImporter.hpp"
#include <Core/VarTypes/Base.hpp>
#include <Resources/ModelParser.hpp>
#include "AssimpMeshLoader.h"

#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Windows/BrowserWindow.hpp"

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::ImportFile(const std::string& str) {
    FileBrowserWindow* fbw = ImGuiLayer::Get()->GetWindow<FileBrowserWindow>();
    std::string cur_dir = fbw->GetCurrentDir();

    uint32 filename_pos = 0;
    uint32 ext_pos = 0;

    for (uint32 i = 0; i < str.size(); i++) {
        char cur_char = str[i];
        if (cur_char == '\\' || cur_char == '/')
            filename_pos = i;
        if (cur_char == '.')
            ext_pos = i;
    }

    std::string res_name = "";
    std::string ext = "";

    for (uint32 i = filename_pos + 1; i < ext_pos; i++) {
        char cur_char = str[i];
        res_name.push_back(cur_char);
    }
    for (uint32 i = ext_pos + 1; i < str.size(); i++) {
        ext.push_back(str[i]);
    }

    if (ext == "dds" || ext == "png") {

    }
    if (ext == "fbx") {
        uint32 num_meshes = 0;
        uint32 num_anims = 0;
        uint32 num_textures = 0;
        uint32 num_materials = 0;

        SceneFileExport exporter;
        //Get amount of meshes, animations, textures, materials in file
        getSizes(str, &num_meshes, &num_anims, &num_textures, &num_materials);

        MeshContainer* meshes = new MeshContainer[num_meshes];

        SceneNode rootNode;
        for (uint32 mesh_i = 0; mesh_i < num_meshes; mesh_i++) {
            loadMesh(str, &meshes[mesh_i], static_cast<int>(mesh_i));
            //Add loaded mesh to exporter
            exporter.pushMesh(&meshes[mesh_i]);
        }

        if (num_meshes > 0) {
            loadNodeTree(str, &rootNode);
            //Set root node to exporter
            exporter.setRootNode(&rootNode);

            std::string new_file_name = cur_dir + "/" + res_name + ".vs3m";
            //Write 3D model scene
            exporter.write(new_file_name);
            //Free all meshes
            delete[] meshes;
            //Register new resource file
        }
    }
}