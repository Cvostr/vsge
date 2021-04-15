#pragma once

#include <Core/VarTypes/Base.hpp>
#include <SDL2/SDL.h>

namespace VSGE {
	class Window {
	private:
		int32 mWindowWidth;
		int32 mWindowHeight;
		int32 mWindowPosX;
		int32 mWindowPosY;

		SDL_Window* mWindow;
	public:
		Window() : mWindow(nullptr),
					mWindowPosX(SDL_WINDOWPOS_CENTERED),
					mWindowPosY(SDL_WINDOWPOS_CENTERED),

					mWindowWidth(1024),
					mWindowHeight(768)
		{}

		SDL_Window* GetSdlWinPtr() { return mWindow; }

		void SetWindowSize(int32 Width, int32 Height);
		void SetPosition(int32 posX, int32 posY);
		void SetWindowTitle(BaseString title);
		void SetResizeable(bool resizeable);
		void SetWindowMode(uint32 mode);

		int32 GetWindowWidth() { return mWindowWidth; }
		int32 GetWindowHeight() { return mWindowHeight; }

		void CreateWindow(int32 Width, int32 Height, BaseString Title, uint32 sdl_win_mode = SDL_WINDOW_OPENGL);
		void DestroyWindow();
	};
}