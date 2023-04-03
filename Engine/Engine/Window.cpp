#include "Window.hpp"
#include <Core/Logger.hpp>
#include "Application.hpp"
#include <System/PlatformSpecific.hpp>
#include <SDL2/SDL_syswm.h>

#undef CreateWindow

using namespace VSGE;

Window* Window::_this = nullptr;

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

void VSGE::Window::CreateWindow(int32 width, int32 height, BaseString title) {
    mWindowWidth = width;
    mWindowHeight = height;

    WIN32_DisableHIDPIWindowScale();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) //Trying to init SDL
    {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error opening window " << SDL_GetError() << "\n";
    }

    Application* app = Application::Get();
    ApplicationCreateInfo desc = app->GetDescription();
    if (!desc.headless) {
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);
        Logger::Log() << "Opening SDL2 window\n";

        uint32 winMode = 0;
        if (desc.graphicsApi == GRAPHICS_API_VULKAN)
            winMode = SDL_WINDOW_VULKAN;
        
        mWindow = SDL_CreateWindow(title, mWindowPosX, mWindowPosY, width, height, winMode); //Create window
    }
}

KeyCode GetKeyCode(int sdl_code) {
    switch (sdl_code) {
    case SDLK_LCTRL:
        return KEY_CODE_LCTRL;
    case SDLK_RCTRL:
        return KEY_CODE_RCTRL;
    case SDLK_TAB:
        return KEY_CODE_TAB;
    case SDLK_LSHIFT:
        return KEY_CODE_LSHIFT;
    case SDLK_LALT:
        return KEY_CODE_LALT;
    case SDLK_RALT:
        return KEY_CODE_RALT;
    case SDLK_BACKSPACE:
        return KEY_CODE_BACKSPACE;
    case SDLK_SPACE:
        return KEY_CODE_SPACE;
    case SDLK_ESCAPE:
        return KEY_CODE_ESCAPE;

    case SDLK_UP:
        return KEY_CODE_UP;
    case SDLK_DOWN:
        return KEY_CODE_DOWN;
    case SDLK_LEFT:
        return KEY_CODE_LEFT;
    case SDLK_RIGHT:
        return KEY_CODE_RIGHT;
    case SDLK_DELETE:
        return KEY_CODE_DELETE;

    case SDLK_F1:
        return KEY_CODE_F1;
    case SDLK_F2:
        return KEY_CODE_F2;
    case SDLK_F3:
        return KEY_CODE_F3;
    case SDLK_F4:
        return KEY_CODE_F4;
    case SDLK_F5:
        return KEY_CODE_F5;
    case SDLK_F6:
        return KEY_CODE_F6;
    case SDLK_F7:
        return KEY_CODE_F7;
    case SDLK_F8:
        return KEY_CODE_F8;
    case SDLK_F9:
        return KEY_CODE_F9;
    case SDLK_F10:
        return KEY_CODE_F10;
    case SDLK_F11:
        return KEY_CODE_F11;
    case SDLK_F12:
        return KEY_CODE_F12;
    }
    return (KeyCode)sdl_code;
}

void VSGE::Window::PollEvents() {
    if (!mWindow)
        return;
    Application* app = Application::Get();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        LibraryWindowEvent libevent;
        libevent.ptr = &event;
        app->OnEvent(libevent);

        if (event.type == SDL_MOUSEMOTION) {
            EventMouseMotion mouse_motion_event(event.motion.x,
                event.motion.y,
                event.motion.xrel,
                event.motion.yrel);
            app->OnEvent(mouse_motion_event);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            EventMouseButtonDown mouse_btndown_event(event.button.button);
            app->OnEvent(mouse_btndown_event);
        }
        if (event.type == SDL_MOUSEBUTTONUP) {
            EventMouseButtonUp mouse_btnup_event(event.button.button);
            app->OnEvent(mouse_btnup_event);
        }
        if (event.type == SDL_MOUSEWHEEL) {
            EventMouseScrolled mouse_scrolled_event(event.wheel.x, event.wheel.y);
            app->OnEvent(mouse_scrolled_event);
        }
        if (event.type == SDL_KEYDOWN) {
            EventKeyButtonDown keyboard_btndown_event(GetKeyCode(event.key.keysym.sym));
            app->OnEvent(keyboard_btndown_event);
        }
        if (event.type == SDL_KEYUP) { //if user pressed a key on keyboard
            EventKeyButtonUp keyboard_btnup_event(GetKeyCode(event.key.keysym.sym));
            app->OnEvent(keyboard_btnup_event);
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                EventWindowResized event_win_resized(
                    event.window.data1,
                    event.window.data2,
                    mWindowWidth,
                    mWindowHeight
                );

                mWindowWidth = event.window.data1;
                mWindowHeight = event.window.data2;
                
                app->OnEvent(event_win_resized);
            }
            if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                EventWindowMinimized event_win_minimized;
                app->OnEvent(event_win_minimized);
            }
            if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {

            }
            if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
                EventWindowRestored event_win_restored;
                app->OnEvent(event_win_restored);
            }
        }
        if (event.type == SDL_QUIT) {
            EventWindowClose event_win_close;
            app->OnEvent(event_win_close);
        }
    }

}

void VSGE::Window::DestroyWindow() {
	if (mWindow) {
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
}

void* VSGE::Window::GetHWND() {
	void* result = nullptr;
#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(this->mWindow, &wmInfo);
	result = wmInfo.info.win.window;
#endif
    return result;
}
