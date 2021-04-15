#pragma once

#include "VarTypes/Base.hpp"

namespace VSGE {
	class TimePerf {
	private:
		uint64 mLast;
		uint64 mNow;
		uint64 mFramesCount;
		float DeltaTime;
	public:
		TimePerf();
		void Tick();
		float GetDeltaTime();
		uint64 GetFramesCount();
	};
}