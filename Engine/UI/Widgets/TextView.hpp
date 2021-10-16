#pragma once

#include "../View.hpp"
#include <Core/VarTypes/String.hpp>
#include <Math/Color.hpp>
#include <Resources/ResourceReference.hpp>

namespace VSGE {
	class TextView : public View {
	private:
		String _text;
		Color _text_color;
		std::string _font_name;
	public:

		TextView();
		~TextView();

		void SetText(const String& text);
		void SetFontName(const std::string& font_name);
		void SetTextColor(const Color& color);

		void Draw();
	};
}