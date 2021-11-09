#include "ApiBindings.hpp"
#include <Engine/Application.hpp>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/debug-helpers.h>
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

void VSGE::BindApplicationApi() {
	mono_add_internal_call("Application::Quit()", Quit);
	mono_add_internal_call("Application::IsHeadless()", IsHeadless);
	mono_add_internal_call("Application::IsServer()", IsServer);

	mono_add_internal_call("Screen::SetResolution(int,int)", SetScreenResolution);
}