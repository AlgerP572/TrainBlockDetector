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
	BlockAxleCount = 0;
	TrainPresent = false;
}

void TrainBlockDetector::AxleDetected(AxleType axleType, float axleScaleSpeedKmH)
{
	TrainAxleCount++;
	BlockAxleCount+=(int) axleType;

	if (TrainAxleCount > 0)
	{
		TrainScaleSpeedKmH = ((TrainScaleSpeedKmH * ((float)TrainAxleCount - 1.0f)) + axleScaleSpeedKmH) / (float)TrainAxleCount;
	}

	if (BlockAxleCount > 0)
	{
		TrainPresent = true;
	}
}

void TrainBlockDetector::Reset()
{
	TrainAxleCount = 0;
	BlockAxleCount = 0;
	TrainPresent = 0;
	TrainScaleSpeedKmH = 0;

	_enterAxleCounter->Reset();
}