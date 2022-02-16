#include "EntityComponents.hpp"
#include "../ViewMaskEdit.hpp"
#include <imgui.h>
#include <Resources/Resource.hpp>
#include "../ResourcePicker.hpp"
#include <Base/ImguiHelper.hpp>

using namespace VSGE;

void VSGEditor::DrawCameraComponent(VSGE::Camera* cac) {
	float near = cac->GetNearPlane();
	DrawFloatControl("Near plane", near);
	cac->SetNearPlane(near);

	float far = cac->GetFarPlane();
	DrawFloatControl("Far plane", far);
	cac->SetFarPlane(far);

	float fov = cac->GetFOV();
	DrawFloatControl("Field of view", fov);
	cac->SetFOV(fov);

	ViewMask current_view_mask = cac->GetViewMask();
	DrawViewMaskEditor(current_view_mask);
	cac->SetViewMask(current_view_mask);

	DrawResourcePicker(std::string("Render Target"), cac->GetTargetReference(), true);

	PostEffectsParams& params = cac->GetPostEffectParams();

	bool bloom_enabled = params._bloom_enabled;
	DrawCheckboxControl("Bloom", bloom_enabled);
	params._bloom_enabled = bloom_enabled;

	bool ssao_enabled = params._ssao_enabled;
	DrawCheckboxControl("SSAO", ssao_enabled);
	params._ssao_enabled = ssao_enabled;
}