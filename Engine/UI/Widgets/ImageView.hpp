#pragma once

#include "../View.hpp"
#include <Resources/ResourceReference.hpp>

namespace VSGE {
	class SpriteView : public View {
	private:
		ResourceReference _texture_reference;
	public:
		SpriteView();
		~SpriteView();

		void SetSprite(const std::string& sprite);
		void Draw();

	};
}