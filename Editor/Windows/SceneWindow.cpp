#include "SceneWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include "InspectorWindow.hpp"

using namespace VSGEditor;
using namespace VSGE;

void SceneWindow::OnDrawWindow() {
    Scene* scene = EditorLayer::Get()->GetScene();

    ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);
    UpdateWindowData();
    //Draw hierarchy from root object
    DrawObjectTreeHierarchy(scene->GetRootEntity());

    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
        if (ImGui::MenuItem("New Entity")) {
            Entity* newEntity = scene->AddNewEntity();
        }
        ImGui::EndPopup();
    }    
    ImGui::End();
}

void SceneWindow::DrawObjectTreeHierarchy(Entity* entity) {
    bool isRoot = entity->GetName().compare("Root") == 0;
    ImGuiTreeNodeFlags flags = 0;
    //If entity has no children, then remove arrow on the left side
    if (entity->GetChildrenCount() == 0)
        flags |= ImGuiTreeNodeFlags_Leaf;
    if (isRoot)//|| (Entity->GetChildrenCount() == 1 && Entity->GetParent()->GetChildrenCount() == 1))
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    //If object is inactive, then draw its name with grey color
    if (!entity->IsActive()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1));
    }
    //Draw Tree Node
    bool tree_open = ImGui::TreeNodeEx(entity->GetName().c_str(), flags);
    if (!isRoot) {
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity")) {
                entity->Destroy();
                InspectorWindow::Get()->SetShowingEntity(nullptr);
            }
            if (ImGui::MenuItem("Dublicate")) {
                //Entity->Dublicate();
            }
            ImGui::EndPopup();
        }
    }
    //If user clicked on Item
    if (ImGui::IsItemClicked() && !isRoot)
    {
        InspectorWindow::Get()->SetShowingEntity(entity);
    }
    //Return back to white color
    if (!entity->IsActive()) {
        ImGui::PopStyleColor();
    }
    ImGuiDragDropFlags src_flags = 0;
    src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
    src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
    if (!isRoot) {
        if (ImGui::BeginDragDropSource(src_flags))
        {
            //Add pop-up text
            ImGui::Text("Moving \"%s\"", entity->GetName().c_str());
            //Push drag_drop data
            Guid id = entity->GetGuid();
            ImGui::SetDragDropPayload("OBJ_SORT", &id.Values, sizeof(uint32) * 4);
            ImGui::EndDragDropSource();
        }
    }

    if (ImGui::BeginDragDropTarget())
    {
        ImGuiDragDropFlags target_flags = 0;

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJ_SORT", target_flags))
        {
            Scene* scene = EditorLayer::Get()->GetScene();
            //Get ID of moving entity
            uint32* move_from = (uint32*)payload->Data;
            Guid id;
            memcpy(id.Values, move_from, sizeof(uint32) * 4);
            //Get pointer to moving entity
            Entity* from = scene->GetEntityWithGuid(id);
            //Remove this entity from parent
            from->GetParent()->RemoveChild(from);
            //Add new child to object
            entity->AddChild(from);
        }
        ImGui::EndDragDropTarget();
    }
    if (tree_open) {
        //Da all chlden
        for (uint32 child_i = 0; child_i < entity->GetChildrenCount(); child_i++) {
            Entity* child_entity = entity->GetChild(child_i);
            DrawObjectTreeHierarchy(child_entity);
        }
        ImGui::TreePop();
    }
}