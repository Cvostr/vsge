#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>
#include <System/FileWatcher.hpp>
#include <Resources/ResourceCache.hpp>

namespace VSGEditor {

	class EditorSettings {

	};

	class EditorLayer : public VSGE::IApplicationLayer {
	private:
		static EditorLayer* _this;
		VSGE::Scene* mScene;
		VSGE::FileWatcher* mResourcesWatcher;
		VSGE::ResourceCache* mResourceCache;
	public:

		EditorLayer() {
			mScene = new VSGE::Scene;
			mScene->NewScene();
			_this = this;
			mResourcesWatcher = new VSGE::FileWatcher;
			mResourceCache = new VSGE::ResourceCache;
		}

		static EditorLayer* Get() {
			return _this;
		}

		VSGE::Scene* GetScene() {
			return mScene;
		}

		VSGE::ResourceCache* GetResourceCache() {
			return mResourceCache;
		}

		void OpenProjectDirectory(const std::string& dir_path);

		void OnAttach();
		void OnUpdate();
		void OnDetach();

	};
}