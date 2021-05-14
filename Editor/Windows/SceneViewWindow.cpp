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
            Mat4 view = GetViewRH(cam->GetPosition(), cam->GetPosition() + cam->GetFront(), cam->GetUp());
            Mat4 offset = Mat4(1);
            Mat4 newmat = editor_layer->GetPickedEntity()->GetWorldTransform();
            float snapValues[3] = { 0.5f, 0.5f, 0.5f };
            ImGuizmo::OPERATION op = ImGuizmo::OPERATION(editor_layer->GetTransformMode());
            bool snap = (op == ImGuizmo::OPERATION::ROTATE);
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(_pos.x, _pos.y, _size.x, _size.y);
            ImGuizmo::Manipulate(&view.M11, &proj.M11,
                op, ImGuizmo::LOCAL, &newmat.M11,
                nullptr, snap ? snapValues : nullptr);


            if (ImGuizmo::IsUsing()) {
                Vec3 translation = newmat.GetPosition();
                Vec3 rotation = newmat.GetRotation();
                Vec3 scale = newmat.GetScale();

                Vec3 parent_translation = editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform().GetPosition();
                Vec3 parent_rotation = editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform().GetRotation();
                Vec3 parent_scale = editor_layer->GetPickedEntity()->GetParent()->GetWorldTransform().GetScale();

                translation = translation - parent_translation;
                scale = scale * (parent_scale.Invert());
                rotation = rotation - parent_rotation;

                editor_layer->GetPickedEntity()->SetPosition(translation);
                editor_layer->GetPickedEntity()->SetScale(scale);
                editor_layer->GetPickedEntity()->SetRotation(rotation);
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

void SceneViewWindow::Regroup(uint32 width, uint32 height) {
    SetPos(width / 4, 20);
    SetSize(width / 2, height * 0.66f);
}