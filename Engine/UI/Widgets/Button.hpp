#pragma once

#include "../View.hpp"
#include <Core/VarTypes/String.hpp>
#include <Resources/ResourceReference.hpp>

namespace VSGE {

	enum class ButtonState {
		NORMAL,
		HOVERED,
		HOLD
	};

	class Button : public View {
	private:
		String _text;
		std::string _font_name;
		ResourceReference _normal_reference;
		ResourceReference _hovered_reference;
		ResourceReference _hold_reference;

		ButtonState _state;

		void UpdateState();
	public:

		Button();
		~Button();
		
		void SetText(const String& text);

		void SetCommonSprite(const std::string& sprite);
		void SetHoveredSprite(const std::string& sprite);
		void SetHoldSprite(const std::string& sprite);

		bool IsPressed() const;

		void Draw();
	};
}