#include <functional>

#include "../../../APLPIe/Src/Headers/Gpio.h"
#include "../../../APLPIe/Src/Headers/Timer.h"
#include "./Headers/StopWatch.h"

#include "./Headers/AxleSensor.h"
#include "./Headers/TrainBlockDetector.h"


TrainBlockDetector::TrainBlockDetector(AxleSensor* enterAxleCounter)
{	
	using namespace std::placeholders;

	_enterAxleCounter = enterAxleCounter;
	_enterAxleCounter->AxleDtected = std::bind(&TrainBlockDetector::AxleDetected,
		this,
		_1,
		_2);
	TrainAxleCount = 0;
	TrainPresent = false;
}

void TrainBlockDetector::AxleDetected(AxleType axleType, float axleScaleSpeedKmH)
{
	TrainAxleCount+=(int) axleType;
	_trainScaleSpeedKmH = ((_trainScaleSpeedKmH * ((float)TrainAxleCount - 1.0f)) + axleScaleSpeedKmH) / (float)TrainAxleCount;

	if (TrainAxleCount > 0)
	{
		TrainPresent = true;
	}
}