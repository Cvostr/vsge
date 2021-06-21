#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Core/VarTypes/Base.hpp>
#include <string>
#include <SDL2/SDL.h>

namespace VSGEditor {
	class EditorSettingsLayer : public VSGE::IApplicationLayer {
	private:
		static EditorSettingsLayer* _this;
	public:
		uint32 icons_size;
		std::string _mono_path;

		uint32 _windowWidth;
		uint32 _windowHeight;
		uint32 _windowPosX;
		uint32 _windowPosY;

		static EditorSettingsLayer* Get() {
			return _this;
		}

		EditorSettingsLayer() {
			_this = this;
			icons_size = 64;
			_windowWidth = 1280;
			_windowHeight = 720;
			_windowPosX = SDL_WINDOWPOS_CENTERED;
			_windowPosY = SDL_WINDOWPOS_CENTERED;
			_mono_path = "D:\\Mono";
		}
	
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}