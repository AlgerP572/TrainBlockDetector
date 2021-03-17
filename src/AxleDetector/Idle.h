#pragma once

#include "../../../CPlusPlusStateMchine/src/StateMachine.h"
#include "StatesTriggers.h"
#include "../Headers/AxleSensor.h"

class Idle : public StateTemplate<Idle,
	TRIGGERS,
	(int)TRIGGERS::Count,
	STATES>
{
private:
	AxleSensor* _axleSensor;

	void LeftWheelFirstTriggerGuard(TRIGGERS trigger, Transition<Idle, STATES>& transition);
	void RightWheelFirstTriggerGuard(TRIGGERS trigger, Transition<Idle, STATES>& transition);

public:
	Idle(AxleSensor* axleSensor);
	void EntryAction() override;
	void ExitAction() override;
};
