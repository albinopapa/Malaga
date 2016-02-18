#include "frametimer.h"
#include <float.h>

FrameTimer::FrameTimer() :
	timeMin(FLT_MAX),
	timeMax(0.0f),
	timeSum(0.0f),
	lastMin(0.0f),
	lastMax(0.0f),
	lastAvg(0.0f),
	frameCount(0)
{
}

void FrameTimer::StartFrame()
{
	timer.StartWatch();
}

void FrameTimer::StopFrame()
{
	timer.StopWatch();

	const float frameTime = timer.GetTimeMilli();
	timeSum += frameTime;
	timeMin = min(timeMin, frameTime);
	timeMax = max(timeMax, frameTime);
	frameCount++;
	if (frameCount >= nFramesAvg)
	{
		lastAvg = timeSum / (float)nFramesAvg;
		lastMin = timeMin;
		lastMax = timeMax;
		timeSum = 0.0f;
		timeMin = FLT_MAX;
		timeMax = 0.0f;
		frameCount = 0;
	}
}

float FrameTimer::GetMax()
{
	return lastMax;
}

float FrameTimer::GetMin()
{
	return lastMin;
}

float FrameTimer::GetAvg()
{
	return lastAvg;
}
