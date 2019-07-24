#pragma once

#include <signal.h>
#include <time.h>

class AxleCounter
{
private:
	Gpio* _gpio = NULL;

	int _leftRailPin;
	int _rightRailPin;
	int _leftOutputPin;
	int _rightOutputPin;
	PinState _leftOutputState;
	PinState _rightOutputState;

	int _leftRailCount;
	int _rightRailCount;

	Timer _axelTimer;
	Stopwatch<std::chrono::microseconds> _axelTime;

	// TODO: configuration items...
	float _detectorLengthMm = 10.0;
	float _railroadScale = 22.5;
	float _axelScaleSizeMm = 1000;
	
	static void LeftRailIsr0(void* arg);
	static void RightRailIsr0(void* arg);

	void LeftRailISR();
	void RightRailISR();
	void ResetForNextAxel();

	float CalculateSpeed();

public:
	int AxleCount;
	
	AxleCounter(Gpio* gpio, int leftRailPin, int rightRailPin, int leftOutputPin = -1, int rightOutputPin = -1);
	void SysInit(void);
	void RefreshOutputStatus();

	std::function<void(int, float)> AxleDtected;
};
