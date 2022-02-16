#pragma once

#include <Math/Vec3.hpp>
#include <Math/Color.hpp>
#include <string>

namespace VSGEditor {
	void DrawVec3Control(const std::string& text, Vec3& value, bool drag = false);
	void DrawVec2Control(const std::string& text, Vec2& value, bool drag = false);
	void DrawColorControl(const std::string& text, VSGE::Color& value);
	void DrawFloatControl(const std::string& text, float& value);
	void DrawIntControl(const std::string& text, int& value);
	void DrawCheckboxControl(const std::string& text, bool& value);
	void DrawStringControl(const std::string& text, std::string& value);
	bool DrawComboControl(const std::string& text, const std::string& current);
}