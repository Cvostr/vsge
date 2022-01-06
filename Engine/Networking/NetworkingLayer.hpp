#pragma once

#include <Engine/ApplicationLayer.hpp>

namespace VSGE{

	class NetworkingLayer : public IApplicationLayer {
	private:
		static NetworkingLayer* _this;
	public:

		NetworkingLayer();
		~NetworkingLayer();

		static NetworkingLayer* Get() {
			return _this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

	};
}