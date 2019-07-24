#include <functional>

#include "../../../APLPIe/Src/Headers/Gpio.h"
#include "../../../APLPIe/Src/Headers/Timer.h"
#include "./Headers/StopWatch.h"

#include "./Headers/AxleCounter.h"
#include "./Headers/TrainBlockDetector.h"


TrainBlockDetector::TrainBlockDetector(AxleCounter* enterAxleCounter)
{	
	using namespace std::placeholders;

	_enterAxleCounter = enterAxleCounter;
	_enterAxleCounter->AxleDtected = std::bind(&TrainBlockDetector::CalculateSpeed,
		this,
		_1,
		_2);
	TrainAxleCount = 0;
	TrainPresent = false;
}

void TrainBlockDetector::CalculateSpeed(int axleCount, float axleScaleSpeedKmH)
{
	TrainAxleCount++;
	_trainScaleSpeedKmH = ((_trainScaleSpeedKmH * ((float)TrainAxleCount - 1.0f)) + axleScaleSpeedKmH) / (float)TrainAxleCount;

	if (TrainAxleCount > 0)
	{
		TrainPresent = true;
	}
}