#pragma once

#include <Scene/Scene.hpp>
#include <Resources/ModelParser.hpp>

namespace VSGEditor {
	void AddSubSceneVS3M(VSGE::Scene* scn, const std::string& file);
	VSGE::Entity* addObjectFromNode(VSGE::Scene* w, VSGE::SceneNode* node);
}