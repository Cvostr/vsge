#include "VariantInput.hpp"
#include <imgui.h>
#include <Base/ImguiHelper.hpp>

using namespace VSGEditor;
using namespace ImGui;

void VSGEditor::DrawVariantInput(const std::string& label, Variant& value) {
	const char* label_s = label.c_str();
	switch (value.GetType()) {
	case VALUE_TYPE_INT32:
		DrawIntControl(label_s, value.GetValue<int>());
		break;
	case VALUE_TYPE_FLOAT:
		DrawFloatControl(label_s, value.GetValue<float>());
		break;
	case VALUE_TYPE_DOUBLE:
		InputDouble(label_s, &value.GetValue<double>());
		break;
	case VALUE_TYPE_BOOL:
		DrawCheckboxControl(label_s, value.GetValue<bool>());
		break;
	case VALUE_TYPE_VEC3F:
		DrawVec3Control(label_s, value.GetValue<Vec3>());
		break;
	case VALUE_TYPE_VEC3I:
		InputInt3(label_s, (int*)&value.GetValue<Vec3i>());
		break;
	case VALUE_TYPE_VEC2F:
		InputFloat2(label_s, (float*)&value.GetValue<Vec2>());
		break;
	case VALUE_TYPE_VEC2I:
		InputInt2(label_s, (int*)&value.GetValue<Vec2i>());
		break;
	case VALUE_TYPE_VEC4F:
		InputFloat4(label_s, (float*)&value.GetValue<Vec4>());
		break;
	case VALUE_TYPE_VEC4I:
		InputInt4(label_s, (int*)&value.GetValue<Vec4i>());
		break;
	case VALUE_TYPE_COLOR:
		DrawColorControl(label_s, value.GetValue<VSGE::Color>());
		break;
	}
}