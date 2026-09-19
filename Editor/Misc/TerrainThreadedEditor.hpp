#pragma once
#include <Core/Threading/Thread.hpp>
#include <Math/Ray.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace VSGEditor {

	class TerrainThreadedEditor : public VSGE::Thread {
	private:
		std::mutex q_mtx;
		std::condition_variable queued_event;
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