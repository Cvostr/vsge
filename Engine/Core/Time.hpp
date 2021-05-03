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
		/// <summary>
		/// Time in seconds, spent to draw last frame
		/// </summary>
		/// <returns></returns>
		float GetDeltaTime();
		/// <summary>
		/// Count of drawn frames
		/// </summary>
		/// <returns></returns>
		uint64 GetFramesCount();
	};
}