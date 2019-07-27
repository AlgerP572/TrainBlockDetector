#pragma once

#include "./AxleSensor.h"

class RightWheelSecond : public StateTemplate<RightWheelSecond,
	TRIGGERS,
	(int)TRIGGERS::Count,
	STATES>
{
private:
	AxleSensor* _axleSensor;

	void IdleTriggerGuard(TRIGGERS trigger, Transition<RightWheelSecond, STATES>& transition);

public:
	RightWheelSecond(AxleSensor* axleSensor);
	void EntryAction() override;
	void ExitAction() override;
};
