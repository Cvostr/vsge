#pragma once

#include "../RenderingAPI.hpp"

namespace VSGE {
	class D3D12RAPI : public IRenderingAPI {
	private:

		static D3D12RAPI* _this;
	public:

		void Create(Window* window);

		void Destroy();

		

		D3D12RAPI(){
			_this = this;

			
		}

		static D3D12RAPI* Get() {
			return _this;
		}
	};
}