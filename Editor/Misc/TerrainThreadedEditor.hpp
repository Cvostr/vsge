#pragma once

#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>
#include <Math/Ray.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <queue>

namespace VSGEditor {

	class TerrainThreadedEditor : public VSGE::Thread {
	private:
		VSGE::Mutex* _mutex;
		std::queue<VSGE::Ray> _edit_rays;
		VSGE::TerrainComponent* _terrain;

		void ProcessRay(const VSGE::Ray& ray);
	public:
		TerrainThreadedEditor();
		~TerrainThreadedEditor();

		void THRFunc();

		void SetTerrain(VSGE::TerrainComponent* terrain);
		void QueueRay(const VSGE::Ray& ray);
	};
}