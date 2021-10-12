#pragma once

#include "../View.hpp"
#include <Core/VarTypes/String.hpp>

namespace VSGE {
	class Button : public View {
	private:
		String _text;
	public:

		Button();
		~Button();
	};
}