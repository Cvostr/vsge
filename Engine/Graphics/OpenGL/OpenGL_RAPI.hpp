#pragma once

#include "../RenderingAPI.hpp"

namespace VSGE {
	class OpenGL_RAPI : public IRenderingAPI {
	private:

		static OpenGL_RAPI* _this;

		SDL_GLContext mGlContext;
	public:
		void Create(Window* window);

		void Destroy();

		SDL_GLContext GetSDLContext() {
			return mGlContext;
		}

		static OpenGL_RAPI* Get() {
			return _this;
		}

		OpenGL_RAPI() {
			_this = this;
			mGlContext = nullptr;
		}
	};
}