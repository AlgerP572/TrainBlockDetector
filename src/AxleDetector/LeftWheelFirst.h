#pragma once

#include "../CPlusPlusStateMchine/src/StateMachine.h"
#include "StatesTriggers.h"
#pragma once

#include "./AxleSensor.h"

class LeftWheelFirst : public StateTemplate<LeftWheelFirst,
	TRIGGERS,
	(int)TRIGGERS::Count,
	STATES>
{
private:
	AxleSensor* _axleSensor;

	void RightWheelSecondTriggerGuard(TRIGGERS trigger, Transition<LeftWheelFirst, STATES>& transition);

public:
	LeftWheelFirst(AxleSensor* axleSensor);
	void EntryAction() override;
	void ExitAction() override;
};
