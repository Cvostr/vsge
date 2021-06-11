#include "SceneViewWindow.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>
#include "../EditorLayers/EditorLayer.hpp"
#include <Math/MatrixCamera.hpp>

using namespace VSGEditor;

void SceneViewWindow::OnDrawWindow() {
    EditorLayer* editor_layer = EditorLayer::Get();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.f));
    if (Draw("World view", ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar)) {
        ImGui::PopStyleColor();
        ImVec2 c = ImGui::GetCursorPos();
        ImGuizmo::SetDrawlist();

        const ImRect bb(_pos + c, _pos + c + _size);

        if (texture == nullptr) {
            VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
            VSGE::VulkanTexture* out = (VSGE::VulkanTexture*)renderer->GetOutputTexture();
            texture = ImGui_ImplVulkan_AddTexture(renderer->GetAttachmentSampler()->GetSampler(),
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

        if (editor_layer->GetPickedEntity()) {
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
    EditorLayer* editor_layer = EditorLayer::Get();
    editor_layer->GetCamera()->SetAspectRatio(_size.x / _size.y);
}