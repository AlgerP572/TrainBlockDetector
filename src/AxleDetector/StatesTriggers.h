#pragma once

#include "../CPlusPlusStateMchine/src/StateMachine.h"

enum class STATES
{
	NOSTATE = RESERVED_NO_STATE,
	NOSTATECHANGE = RESERVED_NO_STATE_CHANGE,
	IDLE = 0,
	LEFTWHEELFIRST,
	LEFTWHEELSECOND,
	RIGHTWHEELFIRST,	
	RIGHTWHEELSECOND,
	Count
};

enum class TRIGGERS
{
	DEFAULTENTRY = RESERVED_TRIGGER_DEFAULT_ENTRY,
	DEFAULTEXIT = RESERVED_TRIGGER_DEFAULT_EXIT,
	LEFTWHEELSENSE = 0,
	RIGHTWHEELSENSE,
	AXLEDEBOUNCE,
	Count
};
