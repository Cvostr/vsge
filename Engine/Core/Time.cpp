#include "Time.hpp"
#include <SDL2/SDL.h>

using namespace VSGE;

TimePerf::TimePerf() :
	mNow(SDL_GetTicks()),
	mLast(0),
	mFramesCount(0),
	DeltaTime(0.f)
{
}
void TimePerf::Tick() {
	mFramesCount++;
	mLast = mNow;
	mNow = SDL_GetTicks();
	DeltaTime = (float)(mNow - mLast) / 1000.f;
}
float TimePerf::GetDeltaTime() {
	return DeltaTime;
}
uint64 TimePerf::GetFramesCount() {
	return mFramesCount;
}