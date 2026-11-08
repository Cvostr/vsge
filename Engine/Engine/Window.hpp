#pragma once

#include <Core/VarTypes/Base.hpp>
#include <SDL2/SDL.h>

namespace VSGE {

	enum class CursorMode {
		Free = 0,		// Default
		Locked = 1,     // for FPS games
		Confined = 2    // for RTS games
	};

	enum class WindowMode {
		Windowed = 0,
		Fullscreen = 1,
		Borderless = 2
	};

	class Window {
	private:
		static Window* _this;

		int32 mWindowWidth;
		int32 mWindowHeight;
		int32 mWindowPosX;
		int32 mWindowPosY;

		bool mGenerateLibEvent;
		SDL_Window* mWindow;
	public:
		Window() : mWindow(nullptr),
					mWindowPosX(SDL_WINDOWPOS_CENTERED),
					mWindowPosY(SDL_WINDOWPOS_CENTERED),

					mWindowWidth(1024),
					mWindowHeight(768),
						
					// Default to false for in-game optimization
					mGenerateLibEvent(false)
		{
			_this = this;
		}

		static Window* Get() {
			return _this;
		}


		void SetGenerateLibEvent(bool generate);
		SDL_Window* GetSdlWinPtr();
		/// <summary>
		/// Get ms windows platform specific HWND of the window
		/// </summary>
		/// <returns></returns>
		void* GetHWND();
		/// <summary>
		/// Change size of created window
		/// </summary>
		/// <param name="width">new width of window</param>
		/// <param name="height">new height of window</param>
		void SetWindowSize(int32 width, int32 height);
		/// <summary>
		/// Move the window on screen
		/// </summary>
		/// <param name="posX">new x coord of window position</param>
		/// <param name="posY">new y coord of window position</param>
		void SetPosition(int32 posX, int32 posY);
		/// <summary>
		/// Change title of created window
		/// </summary>
		/// <param name="title">new title of the window</param>
		void SetWindowTitle(BaseString title);
		/// <summary>
		/// Set window resizable or not
		/// </summary>
		/// <param name="resizable"></param>
		void SetResizable(bool resizable);
		/// <summary>
		/// Change mode of window
		/// Fullscreen, borderless, common
		/// </summary>
		/// <param name="mode">new mode of window</param>
		void SetWindowMode(WindowMode mode);
		/// <summary>
		/// Change cursor movement behavior
		/// </summary>
		/// <param name="mode">new mode of cursor</param>
		void SetCursorMode(CursorMode mode);
		/// <summary>
		/// Get width of the window
		/// </summary>
		/// <returns>width of the window</returns>
		int32 GetWindowWidth();
		/// <summary>
		/// Get height of the window
		/// </summary>
		/// <returns>height of the window</returns>
		int32 GetWindowHeight();
		/// <summary>
		/// Get X-Coordinate of the window
		/// </summary>
		/// <returns>position of the window</returns>
		int32 GetWindowPositionX();
		/// <summary>
		/// Get Y-Coordinate of the window
		/// </summary>
		/// <returns>position of the window</returns>
		int32 GetWindowPositionY();
		/// <summary>
		/// Create window with specified params
		/// </summary>
		/// <param name="width">width of new window</param>
		/// <param name="height">height of new window</param>
		/// <param name="title">title of new window</param>
		void CreateWindow(int32 width, int32 height, BaseString title);
		/// <summary>
		/// Close created window
		/// </summary>
		void DestroyWindow();
		/// <summary>
		/// Collect all window events and send them to application class
		/// </summary>
		void PollEvents();
	};
}