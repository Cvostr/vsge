#include "ApiBindings.hpp"
#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Engine/Window.hpp>

using namespace VSGE;

static void SetScreenResolution(int32 width, int32 height) {
	Application::Get()->GetWindow().SetWindowSize(width, height);
}

static void SetResizable(bool resizable) {
	Application::Get()->GetWindow().SetResizable(resizable);
}

static void SetFullscreenMode(WindowMode mode) {
	Application::Get()->GetWindow().SetWindowMode(mode);
}

void VSGE::BindScreenApi() 
{
	MonoScriptingLayer::AddInternalCall("Screen::SetResolution(int,int)", SetScreenResolution);
	MonoScriptingLayer::AddInternalCall("Screen::SetResizable(bool)", SetResizable);
	MonoScriptingLayer::AddInternalCall("Screen::SetFullscreenMode(FullscreenMode)", SetFullscreenMode);
}