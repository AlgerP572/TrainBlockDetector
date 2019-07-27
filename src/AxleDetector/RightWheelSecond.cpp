#include "StatesTriggers.h"
#include "RightWheelSecond.h"

RightWheelSecond::RightWheelSecond(AxleSensor* axleSensor) :
	_axleSensor(axleSensor)
{
	AddTriggerGuard(TRIGGERS::AXLEDEBOUNCE, &RightWheelSecond::IdleTriggerGuard);
}

void RightWheelSecond::EntryAction()
{
	_axleSensor->RightWheelSecond();
}

void RightWheelSecond::ExitAction()
{
}

void RightWheelSecond::IdleTriggerGuard(TRIGGERS trigger, Transition<RightWheelSecond, STATES>& transition)
{
	transition.TargetState = STATES::IDLE;
}