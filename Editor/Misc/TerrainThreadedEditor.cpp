#include "TerrainThreadedEditor.hpp"
#include <InspectorInterfaces/EntityComponents/EntityComponents.hpp>
#include <Input/Input.hpp>

using namespace VSGEditor;
using namespace VSGE;

TerrainThreadedEditor::TerrainThreadedEditor() 
{
	_terrain = nullptr;
}
TerrainThreadedEditor::~TerrainThreadedEditor() 
{
	_running = false;
	queued_event.notify_one();
	Stop();
}

void TerrainThreadedEditor::SetTerrain(VSGE::TerrainComponent* terrain) 
{
	_terrain = terrain;
}
void TerrainThreadedEditor::QueueRay(const VSGE::Ray& ray) 
{
	{
		std::lock_guard<std::mutex> q_guard(q_mtx);
		_edit_rays.push(ray);
	}

	queued_event.notify_one();
}
void TerrainThreadedEditor::THRFunc() 
{
	while (_running)
	{
		std::unique_lock<std::mutex> lock(q_mtx);

		// wait for tasks
		queued_event.wait(lock, [this] { return !_edit_rays.empty() || _running == false; });

		if (_running == false) {
			lock.unlock();
			return;
		}

		// obtain next ray
		Ray ray = _edit_rays.front();
		_edit_rays.pop();
		// unlock thread
		lock.unlock();

		// perform ray editing
		if (_terrain)
			ProcessRay(ray);
	}
}
void TerrainThreadedEditor::ProcessRay(const Ray& ray) 
{
	bool clear = Input::Get()->IsKeyHold(KEY_CODE_LSHIFT);
	Vec2i coord = _terrain->GetRayIntersectionTraingle(ray);
	int editMode = GetTerrainEditorMode();

	if (editMode == TERRAIN_EDITOR_EDIT_MODE_HEIGHT && coord.x >= 0) {
		_terrain->ModifyHeight(coord,
			GetTerrainEditorOpacity() / 10.f,
			(uint32)GetTerrainEditorBrushSize(),
			!clear);
	}
	else if (editMode == TERRAIN_EDITOR_EDIT_MODE_TEXTURES && coord.x >= 0) {
		_terrain->ModifyTexture(Vec2i(coord.y, coord.x),
			GetTerrainEditorOpacity(),
			(uint32)GetTerrainEditorBrushSize(),
			GetTerrainEditorTextureIndex());
	}
	else if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_VEGETABLES && coord.x >= 0) {
		_terrain->ModifyVegetable(Vec2i(coord.x, coord.y),
			(uint32)GetTerrainEditorBrushSize(),
			GetTerrainEditorVegetableIndex(),
			!clear);
	}
}