#include "SceneViewWindow.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderingGenerics.hpp>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>
#include "../EditorLayers/EditorLayer.hpp"
#include <Math/MatrixCamera.hpp>
#include <Engine/Application.hpp>

using namespace VSGEditor;

SceneViewWindow::SceneViewWindow() {
    _transform_gizmo_used = false;
    texture = VK_NULL_HANDLE;
}

bool SceneViewWindow::IsTransformGizmoUsed() {
    return _transform_gizmo_used;
}

void SceneViewWindow::OnDrawWindow() {
    EditorLayer* editor_layer = EditorLayer::Get();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.f));
    if (Draw("World view", ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar)) {
        ImGui::PopStyleColor();
        ImGuizmo::SetDrawlist();

        const ImRect bb(_pos, _pos + _size);

        if (texture == nullptr) {
            VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
            VSGE::VulkanTexture* out = (VSGE::VulkanTexture*)renderer->GetFinalPass()->GetFramebuffer()->GetColorAttachments()[0];
            texture = ImGui_ImplVulkan_AddTexture(VSGE::VulkanRenderingGenerics::Get()->GetAttachmentSampler()->GetSampler(),
                out->GetImageView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }else

        ImGui::GetWindowDrawList()->AddImage(
            (void*)texture, bb.Min,
            bb.Max, ImVec2(0, 0), ImVec2(1, 1));
        
        VSGE::Camera* cam = editor_layer->GetCamera();
        Mat4 proj = GetPerspectiveRH_Default(cam->GetFOV(), cam->GetAspectRatio(), cam->GetNearPlane(), cam->GetFarPlane());
        Mat4 identity = Mat4(1);
        //ImGuizmo::DrawGrid(&cam->GetViewMatrix().M11, &proj.M11, &identity.M11, 80.f);

        if (editor_layer->GetPickedEntity() && EditorLayer::Get()->GetEditorCameraMode() == EDITOR_CAMERA_MODE_EDIT_CAMERA) {
            VSGE::Entity* picked_entity = editor_layer->GetPickedEntity();
            Mat4 newmat = picked_entity->GetWorldTransform();

            Quat rotation = GetRotationFromQuat(newmat);

            float snapValues[3] = { 0.5f, 0.5f, 0.5f };
            ImGuizmo::OPERATION op = ImGuizmo::OPERATION(editor_layer->GetTransformMode());
            bool snap = (op == ImGuizmo::OPERATION::ROTATE);
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(_pos.x, _pos.y, _size.x, _size.y);

            Vec3 rotDelta;

            ImGuizmo::ManipulateQuat(&cam->GetViewMatrix().M11, &proj.M11,
                op, ImGuizmo::WORLD, &newmat.M11,
                nullptr, snap ? snapValues : nullptr, nullptr, nullptr, &rotDelta.x);

            if (ImGuizmo::IsUsing()) {
                _transform_gizmo_used = true;

                Mat4 delta_mat = newmat * picked_entity->GetParent()->GetWorldTransform().invert();

                if (op == ImGuizmo::TRANSLATE) {
                    picked_entity->SetPosition(delta_mat.GetPosition());
                }
                if (op == ImGuizmo::SCALE) {
                    picked_entity->SetScale(delta_mat.GetScale());
                }
                if (op == ImGuizmo::ROTATE) {
                    Quat delta;
                    delta.CreateFromEulerAngles(rotDelta);
                    rotation = editor_layer->GetPickedEntity()->GetRotation();
                    picked_entity->SetRotation(rotation * delta);
                }
            }
            else {
                _transform_gizmo_used = false;
            }
        }
        ImGui::End();
    }
}

void SceneViewWindow::OnWindowResize() {
    ImVec2 c = ImGui::GetCursorPos();

    //Отправить событие об изменении размера окна рендеринга
    VSGE::EventSceneViewResized event((int)_size.x, (int)_size.y);
    VSGE::Application::Get()->OnEvent(event);

    if(texture != nullptr){
        //ImGui_ImplVulkan_DestroyTexture(texture);
        texture = nullptr;
    }
}