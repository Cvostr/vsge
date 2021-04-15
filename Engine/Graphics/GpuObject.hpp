#pragma once

namespace VSGE {
	class IGpuObject {
	protected:
		bool mCreated;
	public:

		virtual void Destroy() = 0;

		IGpuObject() : mCreated(false) {}
	};
}