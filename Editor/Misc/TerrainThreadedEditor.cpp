#include "TerrainThreadedEditor.hpp"
#include <InspectorInterfaces/EntityComponents/EntityComponents.hpp>

using namespace VSGEditor;
using namespace VSGE;

TerrainThreadedEditor::TerrainThreadedEditor() {
	_mutex = new Mpi::Mutex;
	_terrain = nullptr;
}
TerrainThreadedEditor::~TerrainThreadedEditor() {
	SAFE_RELEASE(_mutex)
}

void TerrainThreadedEditor::SetTerrain(VSGE::TerrainComponent* terrain) {
	_terrain = terrain;
}
void TerrainThreadedEditor::QueueRay(const VSGE::Ray& ray) {
	_mutex->lock();
	_edit_rays.push(ray);
	_mutex->unlock();
}

void TerrainThreadedEditor::THRFunc() {
    while (_running) {

        if (_edit_rays.size() > 0) {
            _mutex->lock();
            //obtain next ray
            Ray ray = _edit_rays.front();
            _edit_rays.pop();
            //unlock thread
            _mutex->unlock();
            //perform ray editing
			if(_terrain)
				ProcessRay(ray);
        }
        SleepThread(5);
    }
}

void TerrainThreadedEditor::ProcessRay(const Ray& ray) {
	Vec2i coord = _terrain->GetRayIntersectionTraingle(ray);
	if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_HEIGHT && coord.x >= 0) {
		_terrain->ModifyHeight(coord,
			GetTerrainEditorOpacity() / 10.f,
			(uint32)GetTerrainEditorBrushSize());
		_terrain->UpdateVegetables();
	}
	if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_TEXTURES && coord.x >= 0) {
		_terrain->ModifyTexture(Vec2i(coord.y, coord.x),
			GetTerrainEditorOpacity(),
			(uint32)GetTerrainEditorBrushSize(),
			GetTerrainEditorTextureIndex());
	}
	if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_GRASS && coord.x >= 0) {
		_terrain->ModifyGrass(Vec2i(coord.x, coord.y),
			(uint32)GetTerrainEditorOpacity(),
			GetTerrainEditorVegetableIndex());
		_terrain->UpdateVegetables();
	}
}