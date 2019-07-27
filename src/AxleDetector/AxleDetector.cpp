#include "AxleDetector.h"
#include "Idle.h"
#include "LeftWheelFirst.h"
#include "LeftWheelSecond.h"
#include "RightWheelFirst.h"
#include "RightWheelSecond.h"

AxleDetector::AxleDetector(AxleSensor* axleSensor)
{
	Idle* idle = new Idle(axleSensor);
	LeftWheelFirst* leftWheelFirst = new LeftWheelFirst(axleSensor);
	LeftWheelSecond* leftWheelSecond = new LeftWheelSecond(axleSensor);
	RightWheelFirst* rightWheelFirst = new RightWheelFirst(axleSensor);
	RightWheelSecond* rightWheelSecond = new RightWheelSecond(axleSensor);

	AddState(STATES::IDLE, idle);
	AddState(STATES::LEFTWHEELFIRST, leftWheelFirst);
	AddState(STATES::LEFTWHEELSECOND, leftWheelSecond);
	AddState(STATES::RIGHTWHEELFIRST, rightWheelFirst);
	AddState(STATES::RIGHTWHEELSECOND, rightWheelSecond);
}