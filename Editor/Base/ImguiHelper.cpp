#include "ImguiHelper.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

using namespace VSGEditor;

void LabelEx(const char* label) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImVec2 lineStart = ImGui::GetCursorScreenPos();
	const ImGuiStyle& style = ImGui::GetStyle();
	float fullWidth = ImGui::GetContentRegionAvail().x;
	float itemWidth = fullWidth * 0.6f;
	ImVec2 textSize = ImGui::CalcTextSize(label);
	ImRect textRect;
	textRect.Min = ImGui::GetCursorScreenPos();
	textRect.Max = textRect.Min;
	textRect.Max.x += fullWidth - itemWidth;
	textRect.Max.y += textSize.y;

	ImGui::SetCursorScreenPos(textRect.Min);

	ImGui::AlignTextToFramePadding();
	textRect.Min.y += window->DC.CurrLineTextBaseOffset;
	textRect.Max.y += window->DC.CurrLineTextBaseOffset;

	ImGui::ItemSize(textRect);
	if (ImGui::ItemAdd(textRect, window->GetID(label)))
	{
		ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(),
			textRect.Min, textRect.Max, textRect.Max.x,
			textRect.Max.x, label, nullptr, &textSize);

		if (textRect.GetWidth() < textSize.x && ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", label);
	}

	ImVec2 new_pos = textRect.Max;
	new_pos.y -= textSize.y + window->DC.CurrLineTextBaseOffset;
	ImGui::SetCursorScreenPos(new_pos);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth);
}

void DrawControlHeader(const std::string& title) {
	LabelEx(title.c_str());
	ImGui::SameLine();
	ImGui::PushID(title.c_str());
}

void VSGEditor::DrawVec3Control(const std::string& text, Vec3& value, bool drag) {
	DrawControlHeader(text);
	if(drag)
		ImGui::DragFloat3("##v", &value.x);
	else
		ImGui::InputFloat3("##v", &value.x);
	ImGui::PopID();
}

void VSGEditor::DrawVec2Control(const std::string& text, Vec2& value, bool drag) {
	DrawControlHeader(text);
	if (drag)
		ImGui::DragFloat2("##v", &value.x);
	else
		ImGui::InputFloat2("##v", &value.x);
	ImGui::PopID();
}

void VSGEditor::DrawColorControl(const std::string& text, VSGE::Color& value) {
	DrawControlHeader(text);
	ImGui::ColorEdit3("##v", &value.r);
	ImGui::PopID();
}

void VSGEditor::DrawFloatControl(const std::string& text, float& value) {
	DrawControlHeader(text);
	ImGui::InputFloat("##v", &value);
	ImGui::PopID();
}

void VSGEditor::DrawIntControl(const std::string& text, int& value) {
	DrawControlHeader(text);
	ImGui::InputInt("##v", &value);
	ImGui::PopID();
}

void VSGEditor::DrawCheckboxControl(const std::string& text, bool& value) {
	DrawControlHeader(text);
	ImGui::Checkbox("##v", &value);
	ImGui::PopID();
}

void VSGEditor::DrawStringControl(const std::string& text, std::string& value) {
	DrawControlHeader(text);
	ImGui::InputText("##v", &value);
	ImGui::PopID();
}

bool VSGEditor::DrawComboControl(const std::string& text, const std::string& current) {
	DrawControlHeader(text);
	bool result = ImGui::BeginCombo("##v", current.c_str());
	ImGui::PopID();
	return result;
}