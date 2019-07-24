#include "../../../APLPIe/Src/Headers/Gpio.h"
#include "../../../APLPIe/Src/Headers/Timer.h"
#include "./Headers/StopWatch.h"

#include "./Headers/AxleCounter.h"

void AxleCounter::LeftRailIsr0(void* arg)
{
	AxleCounter* pCounter = (AxleCounter*)arg;
	pCounter->LeftRailISR();
}

void AxleCounter::RightRailIsr0(void* arg)
{
	AxleCounter* pCounter = (AxleCounter*)arg;
	pCounter->RightRailISR();
}

AxleCounter::AxleCounter(Gpio* gpio, int leftRailPin, int rightRailPin, int leftOutputPin, int rightOutputPin)
{
	_gpio = gpio;
	AxleCount = 0;	
	_leftRailPin = leftRailPin;
	_rightRailPin = rightRailPin;

	// To not use output pins pass a -1. This lets you use the pins
	// for another purpose if desried.
	_leftOutputPin = leftOutputPin;
	_rightOutputPin = rightOutputPin;

	AxleDtected = NULL;
}

void AxleCounter::SysInit()
{
	_gpio->SetPinMode(_leftRailPin, PinMode::Input);
	_gpio->SetPinMode(_rightRailPin, PinMode::Input);

	_gpio->SetPudMode(_leftRailPin, PudMode::PullUp);
	_gpio->SetPudMode(_rightRailPin, PudMode::PullUp);
	
	_gpio->SetIsr(_leftRailPin,
		IntTrigger::Falling,
		LeftRailIsr0,
		this);
	_gpio->SetIsr(_rightRailPin,
		IntTrigger::Falling,
		RightRailIsr0,
		this);

	if (_leftOutputPin > -1)
	{
		_gpio->SetPinMode(_leftOutputPin, PinMode::Output);
	}
	if (_rightOutputPin > -1)
	{
		_gpio->SetPinMode(_rightOutputPin, PinMode::Output);
	}
}

void AxleCounter::RefreshOutputStatus()
{
	_gpio->WritePin(_leftOutputPin, _leftOutputState);
	_gpio->WritePin(_rightOutputPin, _rightOutputState);
}

void AxleCounter::LeftRailISR()
{
	if (_rightRailCount > 0 &&
		_leftRailCount > 0)
	{

		// Nothing to do waiting for reset
		// to count next axle.
		return;
	}

	_leftRailCount++;
	_leftOutputState = PinState::High;

	// Important: Does not restart if already going...
	_axelTime.Start();

	if (_rightRailCount > 0)
	{
		AxleCount++;

		static float scaleFactor = 3600.0f /*sec/h*/ * 1000.0f /* ms / sec */ / 10000000.0f /* mm/km */;
		float axelSpeedkmH = CalculateSpeed();
		int axleDebounceTime = (int) (scaleFactor * _axelScaleSizeMm / axelSpeedkmH); // msec conversion.

		_axelTimer.Start((char*)"LeftAxel",
			std::bind(&AxleCounter::ResetForNextAxel, this),
			axleDebounceTime,
			0);

		if (AxleDtected != NULL)
		{
			AxleDtected(AxleCount, axelSpeedkmH);
		}
	}
}

void AxleCounter::RightRailISR()
{
	if (_rightRailCount > 0 &&
		_leftRailCount > 0)
	{

		// Nothing to do waiting for reset
		// to count next axle.
		return;
	}

	_rightRailCount++;
	_rightOutputState = PinState::High;
		
	// Important: Does not restart if already going...
	_axelTime.Start();

	if (_leftRailCount > 0)
	{		
		AxleCount--;

		static float scaleFactor = 3600.0f /*sec/h*/ * 1000.0f /* ms / sec */ / 10000000.0f /* mm/km */;
		float axelSpeedkmH = CalculateSpeed();
		int axleDebounceTime = (int)(scaleFactor * _axelScaleSizeMm / axelSpeedkmH); // msec conversion.

		_axelTimer.Start("RightAxel",
			std::bind(&AxleCounter::ResetForNextAxel, this),
			axleDebounceTime,
			0);		

		if (AxleDtected != NULL)
		{
			AxleDtected(AxleCount, axelSpeedkmH);
		}
	}
}

void AxleCounter::ResetForNextAxel()
{
	_leftRailCount = 0;
	_rightRailCount = 0;
	_leftOutputState = PinState::Low;
	_rightOutputState = PinState::Low;
}

float AxleCounter::CalculateSpeed()
{
	_axelTime.Stop();

	static float const scaleFactor = (3600.0 /* sec/hr */ * 1000000.0 /* µsec / sec*/) / 10000000.0 /* mm / km*/;

	// Speed is in km/H ...
	float elapsed = (float) _axelTime.Elapsed().count(); // µSec
	_axelTime.Reset();

	float distance = (_detectorLengthMm * _railroadScale); 
	float result = scaleFactor * distance / elapsed; // To scal to Km/H
	return result;	
}