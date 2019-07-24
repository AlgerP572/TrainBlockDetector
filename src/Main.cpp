#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include "../../../APLPIe/Src/Headers/Clock.h"
#include "../../../APLPIe/Src/Headers/Dma.h"
#include "../../../APLPIe/Src/Headers/Gpio.h"
#include "../../../APLPIe/Src/Headers/Pwm.h"
#include "../../../APLPIe/Src/Headers/Timer.h"
#include "../../../APLPIe/Src/Headers/Display.h"

#include "./Headers/StopWatch.h"
#include "./Headers/AxleCounter.h"
#include "./Headers/TrainBlockDetector.h"

// Display (These select the digit to display.)
#define   DisplayPin0    2
#define   DisplayPin1    3
#define   DisplayPin2    8
#define   DisplayPin3    7

#define   DisplayCharPin0 17
#define   DisplayCharPin1 18
#define   DisplayCharPin2 27
#define   DisplayCharPin3 22
#define   DisplayCharPin4 23
#define   DisplayCharPin5 24
#define   DisplayCharPin6 25
#define   DisplayCharPin7 4

static CharacterDisplayPins characterPins = CharacterDisplayPins(DisplayCharPin0,
	DisplayCharPin1,
	DisplayCharPin2,
	DisplayCharPin3,
	DisplayCharPin4,
	DisplayCharPin5,
	DisplayCharPin6,
	DisplayCharPin7);

// Output pulses for testing
//#define LeftRailOutput 21
//#define LeftRailOutput 22

// AxleCounter
#define LeftRailInput    16
#define RightRailInput   12

#define LeftRailOutput 19
#define RightRailOutput 26

static Dma dma("Dma Controller");
static Gpio gpio("Gpio Controller");
static Pwm pwm("Pwm Controller");
static Clock clockController("Clock Controller");

const int numPeripherals = 4;
static Peripheral** peripherals = new Peripheral*[numPeripherals];

static FourDigitSevenSegmentDisplay display(gpio,
	DisplayPin0,
	DisplayPin1,
	DisplayPin2,
	DisplayPin3,
	characterPins);

const int numAxleCounters = 1;
static AxleCounter axleCounters[numAxleCounters] =
{
	AxleCounter(&gpio,
		LeftRailInput,
		RightRailInput,
		LeftRailOutput,
		RightRailOutput)
};

const int numTrainBlockDtectors = 1;

static TrainBlockDetector trainBlockDetector[numTrainBlockDtectors] =
{
	TrainBlockDetector(&(axleCounters[0]))
};

void sysInit(void)
{
	peripherals[0] = &dma;
	peripherals[1] = &gpio;
	peripherals[2] = &pwm;
	peripherals[3] = &clockController;
	
	// Devices depend on peripherals so init them first.
	for (int i = 0; i < numPeripherals; i++)
	{
		peripherals[i]->SysInit();
	}

	gpio.Export(LeftRailInput);
	gpio.Export(RightRailInput);

	gpio.Export(LeftRailOutput);
	gpio.Export(RightRailOutput);

	gpio.Export(DisplayPin0);
	gpio.Export(DisplayPin1);
	gpio.Export(DisplayPin2);
	gpio.Export(DisplayPin3);

	gpio.Export(DisplayCharPin0);
	gpio.Export(DisplayCharPin1);
	gpio.Export(DisplayCharPin2);
	gpio.Export(DisplayCharPin3);
	gpio.Export(DisplayCharPin4);
	gpio.Export(DisplayCharPin5);
	gpio.Export(DisplayCharPin6);
	gpio.Export(DisplayCharPin7);

	// Now init devices... For now working definition is
	// a device is made up of one or more dependent peripherals
	display.SysInit();
	for (int i = 0; i < numAxleCounters; i++)
	{
		axleCounters[i].SysInit();
	}
}

void sysUninit(void)
{
	gpio.Unexport(LeftRailInput);
	gpio.Unexport(RightRailInput);

	gpio.Unexport(LeftRailOutput);
	gpio.Unexport(RightRailOutput);

	gpio.Unexport(DisplayPin0);
	gpio.Unexport(DisplayPin1);
	gpio.Unexport(DisplayPin2);
	gpio.Unexport(DisplayPin3);

	gpio.Unexport(DisplayCharPin0);
	gpio.Unexport(DisplayCharPin1);
	gpio.Unexport(DisplayCharPin2);
	gpio.Unexport(DisplayCharPin3);
	gpio.Unexport(DisplayCharPin4);
	gpio.Unexport(DisplayCharPin5);
	gpio.Unexport(DisplayCharPin6);
	gpio.Unexport(DisplayCharPin7);

	for (int i = 0; i < numPeripherals; i++)
	{
		peripherals[i]->SysUninit();
	}
}

void sig_handler(int sig)
{
	sysUninit();
}

int main(void)
{
	// Register signals 
	signal(SIGQUIT, sig_handler);
	signal(SIGABRT, sig_handler);

	// Now that library is intialized the individual devices
	// can be intialized.
	sysInit();

	while(1)
	{

		// Need to continuously update the dislpay
		// so it can be seen properly.
		display.SetDisplayValue(axleCounters[0].AxleCount);
		display.Display();

		// For prototype display the state of the inputs
		for (int i = 0; i < numAxleCounters; i++)
		{
			axleCounters[i].RefreshOutputStatus();
		}
	}

	sysUninit();
	return 0;
}
