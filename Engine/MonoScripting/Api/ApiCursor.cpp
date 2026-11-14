#include "ApiBindings.hpp"
#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Engine/Window.hpp>

using namespace VSGE;

static void SetCursorMode(CursorMode mode) {
	Application::Get()->GetWindow().SetCursorMode(mode);
}

void VSGE::BindCursorApi()
{
	MonoScriptingLayer::AddInternalCall("Cursor::SetMode(CursorMode)", SetCursorMode);
}