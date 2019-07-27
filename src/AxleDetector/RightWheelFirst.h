#pragma once

#include "../CPlusPlusStateMchine/src/StateMachine.h"
#include "StatesTriggers.h"
#pragma once

#include "./AxleSensor.h"

class RightWheelFirst : public StateTemplate<RightWheelFirst,
	TRIGGERS,
	(int)TRIGGERS::Count,
	STATES>
{
private:
	AxleSensor* _axleSensor;

	void LeftWheelSecondTriggerGuard(TRIGGERS trigger, Transition<RightWheelFirst, STATES>& transition);

public:
	RightWheelFirst(AxleSensor* axleSensor);
	void EntryAction() override;
	void ExitAction() override;
};
