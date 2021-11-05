#include "ApiBindings.hpp"
#include <Engine/Application.hpp>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/debug-helpers.h>

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

void VSGE::BindApplicationApi() {
	mono_add_internal_call("Application::Quit()", Quit);
	mono_add_internal_call("Application::IsHeadless()", IsHeadless);
	mono_add_internal_call("Application::IsServer()", IsServer);
}