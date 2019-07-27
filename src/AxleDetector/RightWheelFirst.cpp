#include "StatesTriggers.h"
#include "RightWheelFirst.h"

RightWheelFirst::RightWheelFirst(AxleSensor* axleSensor) :
	_axleSensor(axleSensor)
{
	AddTriggerGuard(TRIGGERS::LEFTWHEELSENSE, &RightWheelFirst::LeftWheelSecondTriggerGuard);
}

void RightWheelFirst::EntryAction()
{
	_axleSensor->RightWheelFirst();
}

void RightWheelFirst::ExitAction()
{
}

void RightWheelFirst::LeftWheelSecondTriggerGuard(TRIGGERS trigger, Transition<RightWheelFirst, STATES>& transition)
{
	transition.TargetState = STATES::LEFTWHEELSECOND;
}