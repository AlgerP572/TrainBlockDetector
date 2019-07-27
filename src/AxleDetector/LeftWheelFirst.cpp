#include "StatesTriggers.h"
#include "LeftWheelFirst.h"

LeftWheelFirst::LeftWheelFirst(AxleSensor* axleSensor) :
	_axleSensor(axleSensor)
{	
	AddTriggerGuard(TRIGGERS::RIGHTWHEELSENSE, &LeftWheelFirst::RightWheelSecondTriggerGuard);
}

void LeftWheelFirst::EntryAction()
{
	_axleSensor->LeftWheelFirst();
}

void LeftWheelFirst::ExitAction()
{
}

void LeftWheelFirst::RightWheelSecondTriggerGuard(TRIGGERS trigger, Transition<LeftWheelFirst, STATES>& transition)
{
	transition.TargetState = STATES::RIGHTWHEELSECOND;
}