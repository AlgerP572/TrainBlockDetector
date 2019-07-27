#include "StatesTriggers.h"
#include "Idle.h"


Idle::Idle(AxleSensor* axleSensor)
{
	_axleSensor = axleSensor;

	AddTriggerGuard(TRIGGERS::LEFTWHEELSENSE, &Idle::LeftWheelFirstTriggerGuard);
	AddTriggerGuard(TRIGGERS::RIGHTWHEELSENSE, &Idle::RightWheelFirstTriggerGuard);
}

void Idle::EntryAction()
{
	_axleSensor->ResetForNextAxel();
}

void Idle::ExitAction()
{
}

void Idle::LeftWheelFirstTriggerGuard(TRIGGERS trigger, Transition<Idle, STATES>& transition)
{
	transition.TargetState = STATES::LEFTWHEELFIRST;
}

void Idle::RightWheelFirstTriggerGuard(TRIGGERS trigger, Transition<Idle, STATES>& transition)
{
	transition.TargetState = STATES::RIGHTWHEELFIRST;
}