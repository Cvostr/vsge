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

        const ImRect bb(_pos + c, _pos + c + _size);

        if (texture == nullptr) {
            VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
            VSGE::VulkanTexture* out = (VSGE::VulkanTexture*)renderer->GetOutputTexture();
            texture = ImGui_ImplVulkan_AddTexture(renderer->GetAttachmentSampler()->GetSampler(), out->GetImageView(), VK_IMAGE_LAYOUT_UNDEFINED);
        }else

        ImGui::GetWindowDrawList()->AddImage(
            (void*)texture, bb.Min,
            bb.Max, ImVec2(0, 0), ImVec2(1, 1));
            
        if (editor_layer->GetPickedEntity()) {
            VSGE::Camera* cam = editor_layer->GetCamera();
            Mat4 proj = GetPerspectiveRH_Default(cam->GetFOV(), cam->GetAspectRatio(), cam->GetNearPlane(), cam->GetFarPlane());
            Mat4 newmat = editor_layer->GetPickedEntity()->GetWorldTransform();
            float snapValues[3] = { 0.5f, 0.5f, 0.5f };
            ImGuizmo::OPERATION op = ImGuizmo::OPERATION(editor_layer->GetTransformMode());
            bool snap = false;
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(_pos.x, _pos.y, _size.x, _size.y);

            ImGuizmo::Manipulate(&cam->GetViewMatrix().M11, &proj.M11,
                op, ImGuizmo::LOCAL, &newmat.M11,
                nullptr, snap ? snapValues : nullptr);


            if (ImGuizmo::IsUsing()) {

                if (op == ImGuizmo::TRANSLATE) {
                    Vec3 translation = newmat.GetPosition();
                    Vec3 parent_translation = editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform().GetPosition();
                    translation = translation - parent_translation;
                    editor_layer->GetPickedEntity()->SetPosition(translation);
                }
                if (op == ImGuizmo::SCALE) {
                    Vec3 scale = newmat.GetScale();
                    Vec3 parent_scale = editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform().GetScale();
                    scale = scale * parent_scale.Invert();
                    editor_layer->GetPickedEntity()->SetScale(scale);
                }
                if (op == ImGuizmo::ROTATE) {
                    /*Quat rotation = GetRotationFromQuat(newmat);
                    Quat parent_rotation = GetRotationFromQuat(editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform());
                    rotation = rotation * parent_rotation.Inverse();
                    editor_layer->GetPickedEntity()->SetRotation(rotation);*/

                    Vec3 new_rotation = newmat.GetRotation();
                    Vec3 parent_rotation = editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform().GetRotation();
                    Vec3 rotation = new_rotation - parent_rotation;
                    editor_layer->GetPickedEntity()->SetRotationEuler(rotation);
                }

                editor_layer->GetPickedEntity()->UpdateTransformMatrices();
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