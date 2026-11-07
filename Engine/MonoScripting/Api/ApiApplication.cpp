#include "ApiBindings.hpp"
#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Engine/Window.hpp>

using namespace VSGE;

static void Quit() {
	Application::Get()->Stop();
}

static bool IsHeadless() {
	return Application::Get()->GetDescription().headless;
}

static bool IsServer() {
	return Application::Get()->GetDescription().server;
}

static void SetScreenResolution(int32 width, int32 height) {
	Application::Get()->GetWindow().SetWindowSize(width, height);
}

static void SetResizable(bool resizable) {
	Application::Get()->GetWindow().SetResizable(resizable);
}

void VSGE::BindApplicationApi() {
	MonoScriptingLayer::AddInternalCall("Application::Quit()", Quit);
	MonoScriptingLayer::AddInternalCall("Application::IsHeadless()", IsHeadless);
	MonoScriptingLayer::AddInternalCall("Application::IsServer()", IsServer);

	MonoScriptingLayer::AddInternalCall("Screen::SetResolution(int,int)", SetScreenResolution);
	MonoScriptingLayer::AddInternalCall("Screen::SetResizable(bool)", SetResizable);
}