#pragma once
#include "Timer.h"



class FrameTimer
{
	public:

	FrameTimer();
	void StartFrame();
	void StopFrame();
	float GetMax();
	float GetMin(); 
	float GetAvg();
		
private:
	static const int nFramesAvg = 20;
	Timer timer;
	float timeSum;
	float timeMin;
	float timeMax;
	int frameCount;
	float lastMin;
	float lastMax;
	float lastAvg;
};