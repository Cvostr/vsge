#include "BrowserWindow.hpp"
#include <filesystem>
#include <fstream>
#include <locale>
#include <codecvt>
#include <imgui_internal.h>
#include <Core/FileLoader.hpp>
#include <SDL2/SDL.h>
#include <ImageBtnText.h>
#include <EditorLayers/EditorLayer.hpp>
#include <EditorLayers/ImGuiLayer.hpp>
#include <Misc/SceneExt.hpp>
#include <Resources/ResourceTypes/SceneResource.hpp>
#include <Resources/ResourceCache.hpp>
#include "InspectorWindow.hpp"
#include <Resources/ResourceTypes/MaterialResource.hpp>
#include <Misc/Thumbnails.hpp>
#include <Misc/DialogWindows.hpp>
#include <Misc/EditorIcons.hpp>
#include <Misc/VkMaterialsThumbnails.hpp>
#include <MonoScripting/MonoScriptStorage.hpp>
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

namespace fs = std::filesystem;
using namespace VSGEditor;
using namespace VSGE;

void FileBrowserWindow::cd_up() {
    if (_currentDir.compare(this->_rootDir) == 0)
        return;

    std::string newdir = _currentDir;
    while (newdir[newdir.size() - 1] != '\\' && newdir[newdir.size() - 1] != '/') {
        newdir.pop_back();
    }
    //remove \\ from end
    newdir.pop_back();
    SetDirectory(newdir);
}
void FileBrowserWindow::cd(std::string DirName) {
    std::string divisor = "/";
#ifdef _WIN32
    divisor = '\\';
#endif

    SetDirectory(_currentDir + divisor + DirName);
}
void FileBrowserWindow::SetDirectory(std::string Dir) {
    _currentDir = Dir;
    UpdateDirectoryContent();
}

void FileBrowserWindow::RemoveFile(const std::string& path) {
    if (fs::is_directory(path)) {
        fs::remove_all(path);
    }else
        fs::remove(path);
}

void FileBrowserWindow::UpdateDirectoryContent() {
    _files.clear();

    for (const auto& entry : fs::directory_iterator(_currentDir)) {
        FileEntry e;
        //if file starts with "." then don't show it
        if (entry.path().filename().string()[0] == '.')
            continue;
        e.isDir = entry.is_directory();
        e.abs_path = entry.path().string();
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
        std::wstring fname_wstr = entry.path().filename().wstring();
        e.name = convert.to_bytes(fname_wstr);
        e.ext = entry.path().extension().string();
        e.directory = _currentDir;
        //Calculating relative path
        for (uint32 s = (uint32)_rootDir.size() + 1; s < (uint32)e.abs_path.size(); s++) {
            e.rel_path += e.abs_path[s];
        }
        //Push new entry to array
        _files.push_back(e);
    }
}

FileBrowserWindow::FileBrowserWindow(std::string RootDir) {
    _rootDir = RootDir;
    _itemsSize = 64;
    SetDirectory(RootDir);

    EditorIcons::Get()->LoadIcons();
    renaming_entry = nullptr;
    openRenameDialog = false;
}

void FileBrowserWindow::OpenFile(const FileEntry& Entry) {
    VSGE::Resource* resource = nullptr;
    if (Entry.ext == ".scn" && MonoScriptStorage::Get()->IsScriptingReady() && !SceneLayer::Get()->IsSceneLoading()) {
        InspectorWindow* insp = ImGuiLayer::Get()->GetWindow<InspectorWindow>();
        insp->SetShowingEntity(nullptr);

        EditorLayer* el = EditorLayer::Get();
        el->SetPickedEntity(nullptr);
        el->GetOpenedSceneFile() = Entry.abs_path;

        SceneLayer::Get()->GetMainScene()->NewScene();
        resource = ResourceCache::Get()->GetResourceWithFilePath(Entry.abs_path);
        ((SceneResource*)resource)->LoadAsync();
    }
    else if((resource = ResourceCache::Get()->GetResourceWithFilePath(Entry.abs_path)) != nullptr) {
        InspectorWindow* insp = ImGuiLayer::Get()->GetWindow<InspectorWindow>();
        insp->SetShowingResource(resource);
    }
    else{
#ifdef _WIN32
        ShellExecute(0, 0, Entry.abs_path.c_str(), 0, 0, SW_SHOW);
#endif
    }
}

void FileBrowserWindow::OnDrawWindow() {
    Draw("File Browser");

    ImGuiWindow* w = ImGui::GetCurrentWindow(); //Newly created window

    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
        if (ImGui::MenuItem("Refresh")) {
            UpdateDirectoryContent();
        }
        bool create_new_resource = false;
        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Scene")) {
                new_resource_type = RESOURCE_TYPE_SCENE;
                create_new_resource = true;
            }
            if (ImGui::MenuItem("Material")) {
                new_resource_type = RESOURCE_TYPE_MATERIAL;
                create_new_resource = true;
            }
            if (ImGui::MenuItem("Texture")) {
                new_resource_type = RESOURCE_TYPE_TEXTURE;
                create_new_resource = true;
            }
            if (ImGui::MenuItem("Script")) {
                create_new_resource = true;
                new_resource_type = RESOURCE_TYPE_SCRIPT;
            }
            ImGui::EndMenu();
        }

        ImGui::EndPopup();

        if (create_new_resource) {
            rename_file_name = "";
            ImGui::OpenPopup("Resource creation");
        }
    }

    bool renameFile = false;

    if (ImGui::ImageButton(EditorIcons::Get()->GetBackBtnIcon(), ImVec2(20, 20)))
        cd_up();

    std::string dir_name = "";
    std::string path_to_go = _rootDir;
    std::string cur_dir_relative = _currentDir.substr(_rootDir.size());
    for(uint32 path_i = 0; path_i < cur_dir_relative.size() + 1; path_i++) {
        path_to_go.push_back(cur_dir_relative[path_i]);
        if (cur_dir_relative[path_i] != '/' && cur_dir_relative[path_i] != '\\' && cur_dir_relative[path_i] != '\0') {
            dir_name.push_back(cur_dir_relative[path_i]);
        }
        else {
            ImGui::SameLine();
            if (dir_name.empty())
                dir_name = "/";
            bool btn_clicked = ImGui::Button(dir_name.c_str());
            dir_name.clear();
            if (btn_clicked) {
                path_to_go.pop_back();
                SetDirectory(path_to_go);
            }
        }
    }

    ImGui::SameLine();
    ImGui::SliderInt("Size", &_itemsSize, 32, 96);

    float width = w->Size.x;
    
    uint32 drawn_pix = 0;
    for (uint32 f_i = 0; f_i < _files.size(); f_i++) {
        FileEntry* e = &_files[f_i];
        void* icon = EditorIcons::Get()->GetUnknownFileIcon();
        //if file is directory, then set directory icon
        if (e->isDir) icon = EditorIcons::Get()->GetDirIcon();
        if (e->is3dModel()) icon = EditorIcons::Get()->Get3DModelIcon();
        if (e->is3dWorld()) icon = EditorIcons::Get()->GetSceneIcon();
        if (e->isScript()) icon = EditorIcons::Get()->GetScriptIcon();
        if (e->isAudio()) icon = EditorIcons::Get()->GetAudioIcon();

        if (e->isMaterial()) {
            MaterialResource* mat = (MaterialResource*)ResourceCache::Get()->GetResourceWithFilePath(e->abs_path);
            if (mat) {
                if (!mat->IsReady()) {
                    mat->Load();
                }
            }

            ImTextureID thumb = VkMaterialsThumbnails::Get()->GetMaterialThumbnailTexture(mat->GetName());
            if (mat->IsReady())
                VkMaterialsThumbnails::Get()->CreateThumbnail(mat->GetName());
            if (thumb)
               icon = thumb;
        }
        if (e->isTexture()) {
            ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(e->abs_path);
            if (tex)
                icon = tex->imtexture;
        }

        //Draw button with file
        unsigned int pix = 0;
        bool hovered = false;
        bool clicked = ImageButtonWithText(icon, e->name.c_str(),
            &pix, &hovered,
            ImVec2((float)_itemsSize, (float)_itemsSize),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1),
            f_i);
        
        //if user right clicked file
        if (ImGui::BeginPopupContextItem())
        {
            SceneLayer* scene_layer = SceneLayer::Get();
            if (e->is3dModel()) {
                if (ImGui::MenuItem("Add to scene")) {
                    EditorLayer* el = EditorLayer::Get();
                    AddSubSceneVS3M(scene_layer->GetMainScene(), e->abs_path);
                }
            }
            if (e->isPrefab()) {
                if (ImGui::MenuItem("Add to scene")) {
                    EditorLayer* el = EditorLayer::Get();
                    char* out = nullptr;
                    uint32 size = 0;
                    //read prefab file
                    LoadFile(e->abs_path, &out, &size);
                    //Create entities from prefab
                    scene_layer->GetMainScene()->AddFromPrefab((byte*)out, size);
                    //clean memory
                    delete[] out;
                }
            }
            if (ImGui::MenuItem("Open")) {
                OpenFile(*e);
            }
            if (ImGui::MenuItem("Rename")) {
                renaming_entry = e;
                rename_file_name = e->name;
                openRenameDialog = true;
                
            }
            if (ImGui::MenuItem("Delete")) {
                MessageDialogDesc desc;
                desc.dialog_title = "File deletion";
                desc.message = "Are you sure?";
                desc.buttons = MESSAGE_DIALOG_BTN_YES_NO;
                desc.dialog_type = MESSAGE_DIALOG_TYPE_QUESTION;
                DialogUserAction action;
                MessageDialog(&desc, action);
                if(action == DIALOG_USER_ACTION_ACCEPT){
                    RemoveFile(e->abs_path);
                    InspectorWindow* insp = ImGuiLayer::Get()->GetWindow<InspectorWindow>();
                    insp->SetShowingResource(nullptr);
                }
            }
               
            ImGui::EndPopup();         
        }
        
        //if user clicked on file
        if (clicked) {
            if (e->isDir) {
                cd(e->name);
            }
            else
                OpenFile(*e);
        }
        //Summarize pixels
        drawn_pix += pix;
        //If we have enough space
        if ((width - drawn_pix) > 256)
            //Then draw next in same line
            ImGui::SameLine();
        else
            drawn_pix = 0;
    }

    if (openRenameDialog) {
        ImGui::OpenPopup("Rename?");
        openRenameDialog = false;
    }

    if (renaming_entry) {
        RenameFileDialog(renaming_entry);
    }
    CreateResourceDialog();

    //End Window
    ImGui::End();
}

void FileBrowserWindow::RenameFileDialog(FileEntry* Entry) {
    if (ImGui::BeginPopupModal("Rename?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter new name for file");
        ImGui::InputText("##New Name", &rename_file_name);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            fs::rename(Entry->abs_path, Entry->directory + "/" + rename_file_name);
            ImGui::CloseCurrentPopup();
            UpdateDirectoryContent();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

void FileBrowserWindow::CreateResourceDialog() {
    std::string file_extension;
    std::string resource_type_string;

    switch (new_resource_type) {
    case RESOURCE_TYPE_SCENE:
        file_extension = "scn";
        resource_type_string = "Scene";
        break;
    case RESOURCE_TYPE_MATERIAL:
        file_extension = "vsmt";
        resource_type_string = "Material";
        break;
    case RESOURCE_TYPE_TEXTURE:
        file_extension = "vstx";
        resource_type_string = "Texture";
        break;
    case RESOURCE_TYPE_SCRIPT:
        file_extension = "cs";
        resource_type_string = "C# Script";
        break;
    }

    if (ImGui::BeginPopupModal("Resource creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string message = "Enter name for new " + resource_type_string;
        ImGui::Text(message.c_str());
        ImGui::InputText("##New Name", &rename_file_name);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ByteSerialize* content = GetBaseContentToWrite(new_resource_type, rename_file_name);
            rename_file_name += "." + file_extension;
            std::ofstream stream(_currentDir + "/" + rename_file_name, std::ios::binary);
            stream.write((const char*)content->GetBytes(), content->GetSerializedSize());
            stream.close();
            SAFE_RELEASE(content)

            ImGui::CloseCurrentPopup();
            UpdateDirectoryContent();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

VSGE::ByteSerialize* FileBrowserWindow::GetBaseContentToWrite(VSGE::ResourceType res_type, const std::string& res_name) {
    ByteSerialize* serializer = new ByteSerialize;
    if (res_type == RESOURCE_TYPE_SCRIPT) {
        std::string result =
            "using System; \nclass " + res_name + " : EntityScript{\n\tpublic void OnStart() {\n\t}\n\tpublic void OnUpdate() {\n\t}\n}";
        serializer->Serialize(result);
        serializer->PopBack();
    }
    if (res_type == RESOURCE_TYPE_TEXTURE) {
        int size = 256;
        int layers = 1;
        TextureFormat fmt = FORMAT_RGBA;

        serializer->WriteBytes("VSTX", 4);
        serializer->Serialize(size);
        serializer->Serialize(size);
        serializer->Serialize(fmt);
        serializer->Serialize(layers);
    }
    return serializer;
}