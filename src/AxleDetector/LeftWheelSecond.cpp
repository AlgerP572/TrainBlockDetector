#include "StatesTriggers.h"
#include "LeftWheelSecond.h"

LeftWheelSecond::LeftWheelSecond(AxleSensor* axleSensor) :
	_axleSensor(axleSensor)
{
	AddTriggerGuard(TRIGGERS::AXLEDEBOUNCE, &LeftWheelSecond::IdleTriggerGuard);
}

void LeftWheelSecond::EntryAction()
{
	_axleSensor->LeftWheelSecond();
}

void LeftWheelSecond::ExitAction()
{
}

void LeftWheelSecond::IdleTriggerGuard(TRIGGERS trigger, Transition<LeftWheelSecond, STATES>& transition)
{
	transition.TargetState = STATES::IDLE;
}