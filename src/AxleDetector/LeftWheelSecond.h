#pragma once

#include "../Headers/AxleSensor.h"

class LeftWheelSecond : public StateTemplate<LeftWheelSecond,
	TRIGGERS,
	(int)TRIGGERS::Count,
	STATES>
{
private:
	AxleSensor* _axleSensor;

	void IdleTriggerGuard(TRIGGERS trigger, Transition<LeftWheelSecond, STATES>& transition);

public:
	LeftWheelSecond(AxleSensor* axleSensor);
	void EntryAction() override;
	void ExitAction() override;
};
