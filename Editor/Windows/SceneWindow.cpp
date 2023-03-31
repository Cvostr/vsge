#include "SceneWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "InspectorWindow.hpp"
#include "BrowserWindow.hpp"
#include <fstream>

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Scene/EntityComponents/AudioSourceComponent.hpp>
#include <Scene/Camera.hpp>
#include <Scene/SceneLayer.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Resources/DefaultResources.hpp>

using namespace VSGEditor;
using namespace VSGE;
using namespace std;

void SceneWindow::OnDrawWindow() {
    Scene* scene = SceneLayer::Get()->GetMainScene();
    
    if (Draw("Scene Hierarchy")) {
        //clear counter
        _drawn_entities = 0;
        //Draw hierarchy from root object
        DrawEntityTreeHierarchy(scene->GetRootEntity());

        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {

            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Empty Entity")) {
                    Entity* newEntity = scene->AddNewEntity("Entity");
                }
                if (ImGui::MenuItem("Cube")) {
                    Entity* newEntity = scene->AddNewEntity("Cube");
                    MeshComponent* mesh = newEntity->AddComponent<MeshComponent>();
                    mesh->SetMeshName("Cube");
                    MaterialComponent* mat = newEntity->AddComponent<MaterialComponent>();
                    mat->SetMaterialResource(VSGE::getDefaultMaterialResource());
                }
                if (ImGui::MenuItem("Sphere")) {
                    Entity* newEntity = scene->AddNewEntity("Sphere");
                    MeshComponent* mesh = newEntity->AddComponent<MeshComponent>();
                    mesh->SetMeshName("Sphere");
                    MaterialComponent* mat = newEntity->AddComponent<MaterialComponent>();
                    mat->SetMaterialResource(VSGE::getDefaultMaterialResource());
                }
                if (ImGui::MenuItem("Terrain")) {
                    Entity* newEntity = scene->AddNewEntity("Terrain");
                    TerrainComponent* terrain = newEntity->AddComponent<TerrainComponent>();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Directional Light")) {
                    Entity* newEntity = scene->AddNewEntity("Directional Light");
                    LightsourceComponent* light = newEntity->AddComponent<LightsourceComponent>();
                    light->GetLightType() = LIGHT_TYPE_DIRECTIONAL;
                }
                if (ImGui::MenuItem("Point Light")) {
                    Entity* newEntity = scene->AddNewEntity("Point Light");
                    LightsourceComponent* light = newEntity->AddComponent<LightsourceComponent>();
                    light->GetLightType() = LIGHT_TYPE_POINT;
                }
                if (ImGui::MenuItem("Spot light")) {
                    Entity* newEntity = scene->AddNewEntity("Spot Light");
                    LightsourceComponent* light = newEntity->AddComponent<LightsourceComponent>();
                    light->GetLightType() = LIGHT_TYPE_SPOT;
                    light->SetShadowsBias(0.001f);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Particle Emitter")) {
                    Entity* newEntity = scene->AddNewEntity("Particles");
                    ParticleEmitterComponent* light = newEntity->AddComponent<ParticleEmitterComponent>();
                }
                if (ImGui::MenuItem("Audio Source")) {
                    Entity* newEntity = scene->AddNewEntity("Audio");
                    AudioSourceComponent* light = newEntity->AddComponent<AudioSourceComponent>();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Camera")) {
                    Entity* newEntity = scene->AddNewEntity("Camera");
                    Camera* cam = newEntity->AddComponent<Camera>();

                    AudioListenerComponent* listener = newEntity->GetScene()->GetAudioListener();
                    if (!listener) {
                        newEntity->AddComponent<AudioListenerComponent>();
                    }

                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Clear")) {
                InspectorWindow::Get()->SetShowingEntity(nullptr);
                EditorLayer::Get()->SetPickedEntity(nullptr);
                scene->NewScene();
            }

            ImGui::EndPopup();
        }
        ImGui::End();
    }
}

void SceneWindow::DrawEntityTreeHierarchy(Entity* entity) {
    if (_entityRemoved)
        return;
    
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
    string _unique_name = entity->GetName().c_str() + string("##") + to_string(_drawn_entities);
    bool tree_open = ImGui::TreeNodeEx(_unique_name.c_str(), flags);
    if (!isRoot) {
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity")) {
                Entity* picked = EditorLayer::Get()->GetPickedEntity();
                if(picked == entity)
                    EditorLayer::Get()->SetPickedEntity(nullptr);
                InspectorWindow::Get()->SetShowingEntity(nullptr);
                _entityRemoved = true;
            }
            if (ImGui::MenuItem("Dublicate")) {
                entity->Dublicate();
            }
            if (ImGui::MenuItem("Save prefab")) {
                unsigned char* prefab_data = nullptr;
                uint32 size = 0;

                entity->ToPrefab(&prefab_data, size);             

                FileBrowserWindow* fbw = VSGEditor::ImGuiLayer::Get()->GetWindow<FileBrowserWindow>();
                std::string prefab_name = fbw->GetCurrentDir() + "/" + entity->GetName() + ".prefab";
                
                std::ofstream stream(prefab_name, std::ios::binary);
                stream.write((const char*)prefab_data, size);
                stream.close();

                delete[] prefab_data;
            }
            ImGui::EndPopup();
        }
    }
    //Пользователь нажал на элемент дерева
    if (ImGui::IsItemClicked() && !isRoot)
    {
        InspectorWindow::Get()->SetShowingEntity(entity);
        EditorLayer::Get()->SetPickedEntity(entity);
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        EditorLayer* editor = EditorLayer::Get();
        Vec3 pos = entity->GetWorldTransform().GetPosition();
        editor->GetCamera()->SetPosition(pos);
    }

    if (_entityRemoved) {
        entity->Destroy();
        _entityRemoved = false;

        if (tree_open) 
            ImGui::TreePop();

        return;
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
            Scene* scene = SceneLayer::Get()->GetMainScene();
            //Get ID of moving entity
            uint32* move_from = (uint32*)payload->Data;
            Guid id;
            memcpy(id.Values, move_from, sizeof(uint32) * 4);
            //Get pointer to moving entity
            Entity* from = scene->GetEntityWithGuid(id);
            //crash fix
            if (entity->GetParent() != from) {
                //Remove this entity from parent
                from->GetParent()->RemoveChild(from);
                //Add new child to object
                entity->AddChild(from);
            }
        }
        ImGui::EndDragDropTarget();
    }
    if (tree_open) {
        for (uint32 child_i = 0; child_i < entity->GetChildrenCount(); child_i++) {
            Entity* child_entity = entity->GetChildren()[child_i];
            DrawEntityTreeHierarchy(child_entity);
        }
        ImGui::TreePop();
    }
    _drawn_entities++;
}