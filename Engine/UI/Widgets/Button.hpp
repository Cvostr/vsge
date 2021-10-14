#pragma once

#include "../View.hpp"
#include <Core/VarTypes/String.hpp>
#include <Resources/ResourceReference.hpp>

namespace VSGE {
	class Button : public View {
	private:
		String _text;
		ResourceReference _normal_reference;
		ResourceReference _hovered_reference;
		ResourceReference _pressed_reference;
	public:

		Button();
		~Button();
		
		void SetText(const String& text);

		void Draw();
	};
}