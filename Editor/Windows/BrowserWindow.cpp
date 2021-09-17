#include "BrowserWindow.hpp"
#include <filesystem>
#include <fstream>
#include <imgui_internal.h>
#include <Core/FileLoader.hpp>
#include <SDL2/SDL.h>
#include <ImageBtnText.h>
#include <EditorLayers/EditorLayer.hpp>
#include <EditorLayers/ImGuiLayer.hpp>
#include <Misc/SceneExt.hpp>
#include <Scene/SceneSerialization.hpp>
#include <Resources/ResourceCache.hpp>
#include "InspectorWindow.hpp"
#include <Resources/ResourceTypes/MaterialResource.hpp>
#include <Misc/Thumbnails.hpp>
#include <Misc/DialogWindows.hpp>
#include <Misc/EditorIcons.hpp>
#include <Misc/VkMaterialsThumbnails.hpp>

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
        e.name = entry.path().filename().string();
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
}

void FileBrowserWindow::OpenFile(const FileEntry& Entry) {
    VSGE::Resource* resource = nullptr;
    if (Entry.ext == ".scn") {
        InspectorWindow* insp = ImGuiLayer::Get()->GetWindow<InspectorWindow>();
        insp->SetShowingEntity(nullptr);

        EditorLayer* el = EditorLayer::Get();
        el->SetPickedEntity(nullptr);
        el->GetOpenedSceneFile() = Entry.abs_path;
        el->GetScene()->NewScene();

        VSGE::SceneSerializer ss;
        ss.SetScene(el->GetScene());
        ss.Deserialize(Entry.abs_path);


    }
    else if((resource = ResourceCache::Get()->GetResourceWithFilePath(Entry.abs_path)) != nullptr) {
        InspectorWindow* insp = ImGuiLayer::Get()->GetWindow<InspectorWindow>();
        insp->SetShowingResource(resource);
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
        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Scene")) {
                CreateResource(RESOURCE_TYPE_SCENE);
            }
            if (ImGui::MenuItem("Material")) {
                CreateResource(RESOURCE_TYPE_MATERIAL);
            }
            //if (ImGui::MenuItem("Render target")) {
            //    CreateResource(RESOURCE_TYPE_SCENE);
            //}
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    bool renameFile = false;

    if (ImGui::ImageButton(EditorIcons::Get()->GetBackBtnIcon(), ImVec2(20, 20)))
        cd_up();

    std::string dir_name = "";
    std::string path_to_go = "";
    for(uint32 path_i = 0; path_i < _currentDir.size() + 1; path_i++) {
        path_to_go.push_back(_currentDir[path_i]);
        if (_currentDir[path_i] != '/' && _currentDir[path_i] != '\\' && _currentDir[path_i] != '\0') {
            dir_name.push_back(_currentDir[path_i]);
        }
        else {
            ImGui::SameLine();
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
        bool clicked = ImageButtonWithText(icon, e->name.c_str(), &pix, &hovered, ImVec2(_itemsSize, _itemsSize), ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1),
            f_i);
        
        //if user right clicked file
        if (ImGui::BeginPopupContextItem())
        {

            if (e->is3dModel()) {
                if (ImGui::MenuItem("Add to scene")) {
                    EditorLayer* el = EditorLayer::Get();
                    AddSubSceneVS3M(el->GetScene(), e->abs_path);
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
                    el->GetScene()->AddFromPrefab((byte*)out, size);
                    //clean memory
                    delete[] out;
                }
            }
            if (ImGui::MenuItem("Open")) {
                OpenFile(*e);
            }
            if (ImGui::MenuItem("Rename")) {
                MessageDialogDesc desc;
                desc.dialog_title = "File renaming";
                desc.message = "Enter new name for this file?";
                desc.has_text_field = true;
                desc.text_to_edit = e->name;
                desc.buttons = MESSAGE_DIALOG_BTN_YES_NO;
                desc.dialog_type = MESSAGE_DIALOG_TYPE_QUESTION;
                DialogUserAction action;
                MessageDialog(&desc, action);

                if(action == DIALOG_USER_ACTION_ACCEPT){
                    fs::rename(e->abs_path, e->directory + "/" + desc.text_to_edit);
                    UpdateDirectoryContent();
                }
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
                }
            }
               
            ImGui::EndPopup();         
        }

        
        //if user clicked on file
        if (clicked) {
            if (e->isDir) {
                std::string newdir = e->name;
                cd(newdir);
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

    //End Window
    ImGui::End();
}

void FileBrowserWindow::CreateResource(VSGE::ResourceType type){
    std::string file_name;

    std::string file_extension;
    std::string resource_type_string;

    switch(type){
        case RESOURCE_TYPE_SCENE:
            file_extension = "scn";
            resource_type_string = "Scene";
            break;
        case RESOURCE_TYPE_MATERIAL:
            file_extension = "vsmt";
            resource_type_string = "Material";
            break;
    }

    MessageDialogDesc desc;
    desc.dialog_title = "New " + resource_type_string;
    desc.message = "Enter name for new resource file";
    desc.has_text_field = true;
    desc.text_to_edit = file_name;
    desc.buttons = MESSAGE_DIALOG_BTN_YES_NO;
    desc.dialog_type = MESSAGE_DIALOG_TYPE_QUESTION;
    DialogUserAction action;
    MessageDialog(&desc, action);

    if(action == DIALOG_USER_ACTION_ACCEPT){
        desc.text_to_edit += "." + file_extension;
        std::ofstream stream(_currentDir + "/" + desc.text_to_edit, std::ios::binary);
        stream.close();
        UpdateDirectoryContent();
    }
}