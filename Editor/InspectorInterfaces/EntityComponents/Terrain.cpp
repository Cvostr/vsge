#include "EntityComponents.hpp"
#include <imgui.h>
#include <Misc/Thumbnails.hpp>
#include <Windows/ResourcePickWindow.hpp>
#include <EditorLayers/ImGuiLayer.hpp>
#include <ImageBtnText.h>
#include <Base/ImguiHelper.hpp>
#include "../ResourcePicker.hpp"

using namespace VSGEditor;

void VSGEditor::DrawTerrainComponent(VSGE::TerrainComponent* tc) 
{
	int width = tc->GetWidth();
	int length = tc->GetLength();

	DrawIntControl("Terrain Width", width);
	DrawIntControl("Terrain Length", length);
	
	tc->SetWidth(width);
	tc->SetLength(length);
	
	bool castShadows = tc->IsShadowCastEnabled();
	DrawCheckboxControl("Cast shadows", castShadows);
	tc->SetShadowCastEnabled(castShadows);
}