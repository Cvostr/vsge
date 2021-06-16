#pragma once

#include "Resource.hpp"
#include <Engine/Event.hpp>
#include "../Engine/ApplicationLayer.hpp"

namespace VSGE {

	class ResourceLoadEvent : public IEvent {
	private:
		Resource* _resource;
	public:

		Resource* GetResource() const {
			return _resource;
		}

		EVENT_CLASS_TYPE(EventType::EventResourceLoaded)

		ResourceLoadEvent(Resource* resource) {
			_resource = resource;
		}
	};

	class ResourceLayer : public IApplicationLayer {
	public:
		void OnAttach();
		void OnDetach();
		void OnUpdate();

		void OnEvent(const VSGE::IEvent& event);

		void OnResourceEvent(const VSGE::ResourceLoadEvent& event);
	};
}