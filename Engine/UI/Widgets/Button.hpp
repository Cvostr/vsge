#pragma once

#include "../View.hpp"
#include <Core/VarTypes/String.hpp>
#include <Resources/ResourceReference.hpp>

namespace VSGE {
	class Button : public View {
	private:
		String _text;
		std::string _font_name;
		ResourceReference _normal_reference;
		ResourceReference _hovered_reference;
		ResourceReference _pressed_reference;

		void UpdateState();
	public:

		Button();
		~Button();
		
		void SetText(const String& text);

		void SetCommonSprite(const std::string& sprite);
		void SetHoveredSprite(const std::string& sprite);
		void SetPressedSprite(const std::string& sprite);

		bool IsHovered() const;
		bool IsPressed() const;

		void Draw();
	};
}