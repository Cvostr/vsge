#pragma once

#include "VarTypes/Base.hpp"

namespace VSGE {
	class TimePerf {
	private:

		static TimePerf* _this;

		uint64 mLast;
		uint64 mNow;
		uint64 mFramesCount;
		float DeltaTime;
	public:
		TimePerf();
		static TimePerf* Get();
		void Tick();
		float GetDeltaTime();
		uint64 GetFramesCount();
	};
}