#include "BrowserWindow.hpp"
#include <filesystem>
#include <SDL2/SDL.h>
#include <ImageBtnText.h>
#include "../EditorLayers/EditorLayer.hpp"
#include "../Misc/SceneExt.hpp"
#include <Scene/SceneSerialization.hpp>

namespace fs = std::filesystem;
using namespace VSGEditor;

void FileBrowserWindow::cd_up() {
    if (mCurrentDir.compare(this->mRootDir) == 0)
        return;

    std::string newdir = mCurrentDir;
    while (newdir[newdir.size() - 1] != '\\' && newdir[newdir.size() - 1] != '/') {
        newdir.pop_back();
    }
    //remove \\ from end
    newdir.pop_back();
    SetDirectory(newdir);
}
void FileBrowserWindow::cd(std::string DirName) {
    SetDirectory(mCurrentDir + "/" + DirName);
}
void FileBrowserWindow::SetDirectory(std::string Dir) {
    mCurrentDir = Dir;
    UpdateDirectoryContent();
}
void FileBrowserWindow::UpdateDirectoryContent() {
    mFiles.clear();

    for (const auto& entry : fs::directory_iterator(mCurrentDir)) {
        FileEntry e;
        //if file starts with "." then don't show it
        if (entry.path().filename().string()[0] == '.')
            continue;
        e.isDir = entry.is_directory();
        e.abs_path = entry.path().string();
        e.name = entry.path().filename().string();
        e.ext = entry.path().extension().string();
        e.directory = mCurrentDir;
        //Calculating relative path
        for (uint32 s = (uint32)mRootDir.size() + 1; s < (uint32)e.abs_path.size(); s++) {
            e.rel_path += e.abs_path[s];
        }
        //Push new entry to array
        mFiles.push_back(e);
    }
}

FileBrowserWindow::FileBrowserWindow(std::string RootDir) {
    mRootDir = RootDir;
    SetDirectory(RootDir);

    sampler.Create();

    FileIcons.mBackBtnIcon.CreateFromFile("res/icons/dir_back.png", sampler);
    FileIcons.mDirIcon.CreateFromFile("res/icons/dir.png", sampler);
    FileIcons.mUnknownFile.CreateFromFile("res/icons/unknown.png", sampler);
    FileIcons.m3DModelIcon.CreateFromFile("res/icons/3dmodel.png", sampler);
    FileIcons.mSceneIcon.CreateFromFile("res/icons/3d_scene.png", sampler);
}

void FileBrowserWindow::OpenFile(const FileEntry& Entry) {
    if (Entry.ext == ".scn") {
        EditorLayer* el = EditorLayer::Get();
        el->GetScene()->NewScene();

        VSGE::SceneSerializer ss;
        ss.SetScene(el->GetScene());
        ss.Deserialize(Entry.abs_path);
    }
}

void FileBrowserWindow::DeleteFileDialog(FileEntry* Entry) {
    if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Delete file %s ?", Entry->name.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            fs::remove(Entry->abs_path); 
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

void FileBrowserWindow::RenameFileDialog(FileEntry* Entry) {
    if (ImGui::BeginPopupModal("Rename?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("New Name", &new_file_name);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            fs::rename(Entry->abs_path, Entry->directory + "/" + new_file_name);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
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

            }
            if (ImGui::MenuItem("Material")) {

            }
            if (ImGui::MenuItem("Render target")) {

            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    bool deleteFile = false;
    bool renameFile = false;
    

    float width = w->Size.x;
    if (ImGui::ImageButton(FileIcons.mBackBtnIcon.imtexture, ImVec2(64, 64)))
        cd_up();
    unsigned int drawn_pix = 0;
    for (unsigned int f_i = 0; f_i < mFiles.size(); f_i++) {
        FileEntry* e = &mFiles[f_i];
        ImguiVulkanTexture* icon = &FileIcons.mUnknownFile;
        //if file is directory, then set directory icon
        if (e->isDir) icon = &FileIcons.mDirIcon;
        if (e->is3dModel()) icon = &FileIcons.m3DModelIcon;
        if (e->is3dWorld()) icon = &FileIcons.mSceneIcon;
        /*if (e->isMaterial()) {
            PbrMaterial* mat = resources->getMaterialByRelPath(e->rel_path);
            if (mat != nullptr) {
                if (mat->___extra == 0)
                    renderer->RenderMaterialPreview(mat);
                icon = mat->___extra;
            }
        }*/
        if (e->isTexture()) {
            //ResourceManager* m = Application::getApplication().GetLayer<ResourceManager>();
            //D3D11Texture2D* res = m->GetResourceByPath<D3D11Texture2D>(e->rel_path);
            //GLtexture2D* t = resources->getTexture(e->rel_path);
            //icon = t->getTexture();
        }

        //Draw button with file
        unsigned int pix = 0;
        bool hovered = false;
        bool clicked = ImageButtonWithText(icon->imtexture, e->name.c_str(), &pix, &hovered, ImVec2(64, 64));
       
        
        //if user right clicked file
        if (ImGui::BeginPopupContextItem())
        {

            if (e->ext.compare(".vs3m") == 0) {
                if (ImGui::MenuItem("Add to scene")) {
                    EditorLayer* el = EditorLayer::Get();
                    AddSubSceneVS3M(el->GetScene(), e->abs_path);
                }
            }
            if (ImGui::MenuItem("Open")) {
                OpenFile(*e);
            }
            if (ImGui::MenuItem("Rename")) {
                renameFile = true;
                entryToDelete = e;
            }
            if (ImGui::MenuItem("Delete")) {
                deleteFile = true;
                entryToDelete = e;
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
    if (deleteFile) {
        ImGui::OpenPopup("Delete?");
        deleteFile = false;
    }
    if (renameFile) {
        ImGui::OpenPopup("Rename?");
        new_file_name = entryToDelete->name;
        renameFile = false;
    }

    DeleteFileDialog(entryToDelete);
    RenameFileDialog(entryToDelete);


    //End Window
    ImGui::End();
}

void FileBrowserWindow::Regroup(uint32 width, uint32 height) {
    SetPos(0, height * 0.66f);
    SetSize(width, height * 0.33f);
}