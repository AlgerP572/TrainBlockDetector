#pragma once

#include <signal.h>
#include <time.h>
#include "../AxleDetector/AxleDetector.h"
#include "../../../APLPIe/Src/Headers/Gpio.h"
#include "../../../APLPIe/Src/Headers/Timer.h"
#include "./StopWatch.h"

enum class AxleType
{
	RightToLeftAxle = -1,
	UnknownAxle = 0,
	LeftToRightAxle = 1
};

class AxleSensor
{
private:
	Gpio* _gpio = NULL;

	int _leftRailPin;
	int _rightRailPin;
	int _leftOutputPin;
	int _rightOutputPin;
	float _previousSpeed;
	PinState _leftOutputState;
	PinState _rightOutputState;

	Timer _axelTimer;
	Stopwatch<std::chrono::microseconds> _axelTime;

	AxleDetector _axleDetector;

	// TODO: configuration items...
	float _detectorLengthMm = 10.0;
	float _railroadScale = 22.5;
	float _axelScaleSizeMm = 1000;
	
	static void LeftRailIsr0(void* arg);
	static void RightRailIsr0(void* arg);

	void AxleDebounce();

	void LeftRailISR();
	void RightRailISR();
	float CalculateSpeed();

public:	
	AxleSensor(Gpio* gpio, int leftRailPin, int rightRailPin, int leftOutputPin = -1, int rightOutputPin = -1);
	void SysInit(void);
	void RefreshOutputStatus();

	std::function<void(AxleType, float)> AxleDtected;

	void LeftWheelFirst();
	void LeftWheelSecond();
	void RightWheelFirst();
	void RightWheelSecond();
	void ResetForNextAxel();
	void Reset();
};
