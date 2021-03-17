#include "./Headers/AxleSensor.h"
#include "./AxleDetector/StatesTriggers.h"

void AxleSensor::LeftRailIsr0(void* arg)
{
	AxleSensor* pCounter = (AxleSensor*)arg;
	pCounter->LeftRailISR();
}

void AxleSensor::RightRailIsr0(void* arg)
{
	AxleSensor* pCounter = (AxleSensor*)arg;
	pCounter->RightRailISR();
}

AxleSensor::AxleSensor(Gpio* gpio,
	int leftRailPin,
	int rightRailPin,
	int leftOutputPin,
	int rightOutputPin) :
	_axleDetector(this),
	_previousSpeed(0)
{
	_gpio = gpio;
	_leftRailPin = leftRailPin;
	_rightRailPin = rightRailPin;

	// To not use output pins pass a -1. This lets you use the pins
	// for another purpose if desried.
	_leftOutputPin = leftOutputPin;
	_rightOutputPin = rightOutputPin;

	AxleDtected = NULL;
}

void AxleSensor::SysInit()
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

	_axleDetector.Trigger(TRIGGERS::DEFAULTENTRY);
}

void AxleSensor::RefreshOutputStatus()
{
	_gpio->WritePin(_leftOutputPin, _leftOutputState);
	_gpio->WritePin(_rightOutputPin, _rightOutputState);
}

void AxleSensor::LeftRailISR()
{
	_axleDetector.Trigger(TRIGGERS::LEFTWHEELSENSE);
}

void AxleSensor::LeftWheelFirst()
{
//	_leftRailCount++;
	_leftOutputState = PinState::High;

	// Important: Does not restart if already going...
	_axelTime.Start();
}

void AxleSensor::LeftWheelSecond()
{
		static float scaleFactor = 3600.0f /*sec/h*/ * 1000.0f /* ms / sec */ / 1000000.0f /* mm/km */;
		float axelSpeedkmH = CalculateSpeed();

		if ((_previousSpeed != 0) &&
			(abs(axelSpeedkmH - _previousSpeed) >= (0.1f * _previousSpeed)))
		{

			// look for outlier and ignore.  This is likely a linux thread timing issue...
			axelSpeedkmH = _previousSpeed;
		}
		else
		{
			_previousSpeed = axelSpeedkmH;
		}

		int axleDebounceTime = (int)(scaleFactor * _axelScaleSizeMm / axelSpeedkmH); // msec conversion.

		_axelTimer.Start((char*)"LeftAxel",
			std::bind(&AxleSensor::AxleDebounce, this),
			axleDebounceTime,
			0);

		if (AxleDtected != NULL)
		{
			AxleDtected(AxleType::RightToLeftAxle, axelSpeedkmH);
		}
}

void AxleSensor::RightRailISR()
{
	_axleDetector.Trigger(TRIGGERS::RIGHTWHEELSENSE);	
}

void AxleSensor::RightWheelFirst()
{
	_rightOutputState = PinState::High;

	// Important: Does not restart if already going...
	_axelTime.Start();
}

void AxleSensor::RightWheelSecond()
{
		static float scaleFactor = 3600.0f /*sec/h*/ * 1000.0f /* ms / sec */ / 1000000.0f /* mm/km */;
		float axelSpeedkmH = CalculateSpeed();
		
		if ((_previousSpeed != 0) &&
			(abs(axelSpeedkmH - _previousSpeed) >= (0.1f * _previousSpeed)))
		{

			// look for outlier and ignore.  This is likely a linux thread timing issue...
			axelSpeedkmH = _previousSpeed;
		}
		else
		{
			_previousSpeed = axelSpeedkmH;
		}

		int axleDebounceTime = (int)(scaleFactor * _axelScaleSizeMm / axelSpeedkmH); // msec conversion.

		_axelTimer.Start("RightAxel",
			std::bind(&AxleSensor::AxleDebounce, this),
			axleDebounceTime,
			0);

		if (AxleDtected != NULL)
		{
			AxleDtected(AxleType::LeftToRightAxle, axelSpeedkmH);
		}
}

void AxleSensor::AxleDebounce()
{
	_axleDetector.Trigger(TRIGGERS::AXLEDEBOUNCE);	
}

void AxleSensor::Reset()
{
	ResetForNextAxel();
	_previousSpeed = 0;
}

void AxleSensor::ResetForNextAxel()
{
	_leftOutputState = PinState::Low;
	_rightOutputState = PinState::Low;	
}

float AxleSensor::CalculateSpeed()
{
	_axelTime.Stop();

	static float const scaleFactor = (3600.0 /* sec/hr */ * 1000000.0 /* �sec / sec*/) / 1000000.0 /* mm / km*/;

	// Speed is in km/H ...
	float elapsed = (float) _axelTime.Elapsed().count(); // �Sec
	_axelTime.Reset();

	float distance = (_detectorLengthMm * _railroadScale); 
	float result = scaleFactor * distance / elapsed; // To scal to Km/H
	return result;	
}