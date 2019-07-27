#pragma once

#include "StatesTriggers.h"

class AxleSensor;

class AxleDetector : public OrState<AxleDetector,
	TRIGGERS,
	(int)TRIGGERS::Count,
	STATES,
	(int)STATES::Count,
	STATES::IDLE>
{
public:
	AxleDetector(AxleSensor* axleSensor);

	void EntryAction() override {};
	void virtual ExitAction() override {};
};
