#include "Window.hpp"
#include <Core/Logger.hpp>
#include "../Core/Random.hpp"

void VSGE::Window::SetWindowSize(int32 width, int32 height) {
	if (width < 1 && height < 1) 
		return;

	mWindowWidth = width;
	mWindowHeight = height;

	if (mWindow)
		SDL_SetWindowSize(this->mWindow, width, height);
}

void VSGE::Window::SetPosition(int32 posX, int32 posY) {
	if (mWindow)
		SDL_SetWindowPosition(this->mWindow, posX, posY);

	mWindowPosX = posX;
	mWindowPosY = posY;
}

void VSGE::Window::SetWindowTitle(BaseString title) {
	if (mWindow)
		SDL_SetWindowTitle(mWindow, title);
}

void VSGE::Window::SetWindowMode(uint32 mode) {
	if(mWindow)
		SDL_SetWindowFullscreen(this->mWindow, mode);
}

void VSGE::Window::SetResizeable(bool resizeable) {
	if(mWindow)
		SDL_SetWindowResizable(mWindow, SDL_bool(resizeable));
}

void VSGE::Window::CreateWindow(int32 Width, int32 Height, BaseString Title, uint32 sdl_win_mode) {
	mWindowWidth = Width;
	mWindowHeight = Height;
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) //Trying to init SDL
	{
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Error opening window " << SDL_GetError() << "\n";
	}

	SetRandomSeed(SDL_GetTicks());

	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	Logger::Log() << "Opening SDL2 window\n";
	mWindow = SDL_CreateWindow(Title, mWindowPosX, mWindowPosY, Width, Height, sdl_win_mode); //Create window
}

void VSGE::Window::DestroyWindow() {
	if (mWindow) {
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
}