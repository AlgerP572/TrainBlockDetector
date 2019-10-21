#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include "../../../APLPIe/Src/Headers/Clock.h"
#include "../../../APLPIe/Src/Headers/Delay.h"
#include "../../../APLPIe/Src/Headers/Dma.h"
#include "../../../APLPIe/Src/Headers/Gpio.h"
#include "../../../APLPIe/Src/Headers/Pwm.h"
#include "../../../APLPIe/Src/Headers/Timer.h"
#include "../../../APLPIe/Src/Headers/Display.h"

#include "./Headers/StopWatch.h"
#include "./Headers/AxleSensor.h"
#include "./Headers/TrainBlockDetector.h"

#include "./AxleDetector/AxleDetector.h"

#include <iostream>
#include <fstream>
using namespace std;

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
#define LeftAxleEvent 21
#define RightAxleEvent 13

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

const int numAxleSensors = 1;
static AxleSensor axleSensors[numAxleSensors] =
{
	AxleSensor(&gpio,
		LeftRailInput,
		RightRailInput,
		LeftRailOutput,
		RightRailOutput)
};

const int numTrainBlockDetectors = 1;

static TrainBlockDetector trainBlockDetector[numTrainBlockDetectors] =
{
	TrainBlockDetector(&(axleSensors[0]))
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

	gpio.Export(LeftAxleEvent);
	gpio.Export(RightAxleEvent);

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
	for (int i = 0; i < numAxleSensors; i++)
	{
		axleSensors[i].SysInit();
	}

	// no device for this so emulating a sysint
	gpio.WritePin(LeftAxleEvent, PinState::Low);
	gpio.SetPinMode(LeftAxleEvent, PinMode::Output);
	gpio.WritePin(RightAxleEvent, PinState::Low);
	gpio.SetPinMode(RightAxleEvent, PinMode::Output);
}

void sysUninit(void)
{
	gpio.Unexport(LeftRailInput);
	gpio.Unexport(RightRailInput);

	gpio.Unexport(LeftRailOutput);
	gpio.Unexport(RightRailOutput);

	gpio.Unexport(LeftAxleEvent);
	gpio.Unexport(RightAxleEvent);

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

static float const trainScale = 22.5f; // Large scale G or IIm
static float const gapSizemm = 1.6f; // actual size of the sensor gap in the track.
static float const detectorLengthmm = 10.0f; // actual distance between left and right sensor gaps in the track

static float const mmPerkm = 1000.0f * 1000.0; // 1000 mm / m * 1000 m / km
static float const msPerh = 3600.0f * 1000.0;  // 3600 s / h * 1000 ms / s

int LeftFirstTriggerAxle(int gapTimeMicroSec, int detectorLengthMicroSec)
{

	// Left wheel across sensor gap.
	gpio.WritePin(LeftAxleEvent, PinState::High);
	Delay::Microseconds(gapTimeMicroSec);
	gpio.WritePin(LeftAxleEvent, PinState::Low);

	Delay::Microseconds(detectorLengthMicroSec);

	// Both across central gap.
	gpio.WritePin(RightAxleEvent, PinState::High);
	gpio.WritePin(LeftAxleEvent, PinState::High);
	Delay::Microseconds(gapTimeMicroSec);
	gpio.WritePin(RightAxleEvent, PinState::Low);
	gpio.WritePin(LeftAxleEvent, PinState::Low);

	Delay::Microseconds(detectorLengthMicroSec);

	// Right wheel across sensor gap
	gpio.WritePin(RightAxleEvent, PinState::High);
	Delay::Microseconds(gapTimeMicroSec);
	gpio.WritePin(RightAxleEvent, PinState::Low);

	// Left first is the Enter block direction so we count up.
	return 1; // One axle expected to have been triggered.
}

int LeftFirstTrigger2AxleRollingStock(float trainSpeedkmPerh, float carEndToAxlemm, float wheelbasemm)
{
	int gapTimeMicroSec = (int)(float)(((gapSizemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int detectorLengthMicroSec = (int)(float)(((detectorLengthmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);

	int carEndToAxleMircoSec = (int)(float)(((carEndToAxlemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int wheelbaseMicroSec = (int)(float)(((wheelbasemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);

	Delay::Microseconds(carEndToAxleMircoSec);
	int axleCount = LeftFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);

	Delay::Microseconds(wheelbaseMicroSec);

	axleCount += LeftFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);
	Delay::Microseconds(carEndToAxleMircoSec);

	return axleCount; // One axle expected to have been triggered.
}

int LeftFirstTrigger4AxleRollingStock(float trainSpeedkmPerh, float carEndToTruckmm, float truckSeparationmm, float truckWheelbasemm)
{
	int gapTimeMicroSec = (int)(float)(((gapSizemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int detectorLengthMicroSec = (int)(float)(((detectorLengthmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);

	int carEndToTruckMircoSec = (int)(float)(((carEndToTruckmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int truckSeparationMicroSec = (int)(float)(((truckSeparationmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int truckWheelbaseMicroSec = (int)(float)(((truckWheelbasemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int halfTruckWheelbaseMicroSec = truckWheelbaseMicroSec / 2;
	
	Delay::Microseconds(carEndToTruckMircoSec - halfTruckWheelbaseMicroSec);

	int axleCount = LeftFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);
	Delay::Microseconds(truckWheelbaseMicroSec);
	axleCount += LeftFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);

	Delay::Microseconds(truckSeparationMicroSec - truckWheelbaseMicroSec);

	axleCount += LeftFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);
	Delay::Microseconds(truckWheelbaseMicroSec);
	axleCount += LeftFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);

	Delay::Microseconds(carEndToTruckMircoSec - halfTruckWheelbaseMicroSec);

	return axleCount; // One axle expected to have been triggered.
}

int RightFirstTriggerAxle(int gapTimeMicroSec, int detectorLengthMicroSec)
{

	// Right wheel across sensor gap.
	gpio.WritePin(RightAxleEvent, PinState::High);
	Delay::Microseconds(gapTimeMicroSec);
	gpio.WritePin(RightAxleEvent, PinState::Low);

	Delay::Microseconds(detectorLengthMicroSec);

	// Both across central gap.
	gpio.WritePin(RightAxleEvent, PinState::High);
	gpio.WritePin(LeftAxleEvent, PinState::High);
	Delay::Microseconds(gapTimeMicroSec);
	gpio.WritePin(RightAxleEvent, PinState::Low);
	gpio.WritePin(LeftAxleEvent, PinState::Low);

	Delay::Microseconds(detectorLengthMicroSec);

	// Left wheel across sensor gap
	gpio.WritePin(LeftAxleEvent, PinState::High);
	Delay::Microseconds(gapTimeMicroSec);
	gpio.WritePin(LeftAxleEvent, PinState::Low);

	// Right first is the leave block direction so we count down.
	return -1; // One axle expected to have been triggered.
}

int RightFirstTrigger2AxleRollingStock(float trainSpeedkmPerh, float carEndToAxlemm, float wheelbasemm)
{
	int gapTimeMicroSec = (int)(float)(((gapSizemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int detectorLengthMicroSec = (int)(float)(((detectorLengthmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);

	int carEndToAxleMircoSec = (int)(float)(((carEndToAxlemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int wheelbaseMicroSec = (int)(float)(((wheelbasemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);

	Delay::Microseconds(carEndToAxleMircoSec);
	int axleCount = RightFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);

	Delay::Microseconds(wheelbaseMicroSec);

	axleCount += RightFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);
	Delay::Microseconds(carEndToAxleMircoSec);

	return axleCount; // One axle expected to have been triggered.
}

int RightFirstTrigger4AxleRollingStock(float trainSpeedkmPerh, float carEndToTruckmm, float truckSeparationmm, float truckWheelbasemm)
{
	int gapTimeMicroSec = (int)(float)(((gapSizemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int detectorLengthMicroSec = (int)(float)(((detectorLengthmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);

	int carEndToTruckMircoSec = (int)(float)(((carEndToTruckmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int truckSeparationMicroSec = (int)(float)(((truckSeparationmm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int truckWheelbaseMicroSec = (int)(float)(((truckWheelbasemm * trainScale / mmPerkm) / trainSpeedkmPerh) * msPerh * 1000.0);
	int halfTruckWheelbaseMicroSec = truckWheelbaseMicroSec / 2;

	Delay::Microseconds(carEndToTruckMircoSec - halfTruckWheelbaseMicroSec);

	int axleCount = RightFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);
	Delay::Microseconds(truckWheelbaseMicroSec);
	axleCount += RightFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);

	Delay::Microseconds(truckSeparationMicroSec - truckWheelbaseMicroSec);

	axleCount += RightFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);
	Delay::Microseconds(truckWheelbaseMicroSec);
	axleCount += RightFirstTriggerAxle(gapTimeMicroSec, detectorLengthMicroSec);

	Delay::Microseconds(carEndToTruckMircoSec - halfTruckWheelbaseMicroSec);

	return axleCount; // One axle expected to have been triggered.
}


static bool counting = true;
static int expectedAxleCount = 0;
static float speed = 0.0f;

void* AxleEventThread(void* ptr)
{
	gpio.WritePin(LeftAxleEvent, PinState::Low);
	gpio.WritePin(RightAxleEvent, PinState::Low);

	Delay::Milliseconds(5000);	

	expectedAxleCount = 0;

	ofstream outputFile;
	outputFile.open("DetectionData.txt");
	outputFile << "Speed, Expected, Actual\n";
		
	for (speed = 10.0f; speed < 100.0f; speed+=10)
	{

		// 1200 axles total in the entering the blcok
		for (int i = 0; i < 100; i++)
		{
			// 4 axles total
			expectedAxleCount += LeftFirstTrigger2AxleRollingStock(speed, 85.0f, 175.0f);
			expectedAxleCount += LeftFirstTrigger2AxleRollingStock(speed, 85.0f, 175.0f);

			// 8 axles total
			expectedAxleCount += LeftFirstTrigger4AxleRollingStock(speed, 100.0f, 400.0f, 60.0f);
			expectedAxleCount += LeftFirstTrigger4AxleRollingStock(speed, 100.0f, 400.0f, 60.0f);
		}

		int actual = trainBlockDetector[0].TrainAxleCount;
		outputFile << speed << "," << expectedAxleCount << "," << actual << "\n";
				
		Delay::Milliseconds(1000);

		// 1200 axles total leaving the block
		for (int i = 0; i < 100; i++)
		{
			// 4 axles total
			expectedAxleCount += RightFirstTrigger2AxleRollingStock(speed, 85.0f, 175.0f);
			expectedAxleCount += RightFirstTrigger2AxleRollingStock(speed, 85.0f, 175.0f);

			// 8 axles total
			expectedAxleCount += RightFirstTrigger4AxleRollingStock(speed, 100.0f, 400.0f, 60.0f);
			expectedAxleCount += RightFirstTrigger4AxleRollingStock(speed, 100.0f, 400.0f, 60.0f);
		}
		
		// Expect zero at this point...
		actual = trainBlockDetector[0].TrainAxleCount;
		outputFile << speed << "," << expectedAxleCount << "," << actual << "\n";
		outputFile.flush();

		trainBlockDetector[0].Reset();
		Delay::Milliseconds(1000);
	}

	outputFile.close();

	counting = false;
	return NULL;
}

int main(void)
{
	pthread_t generateAxleEventsThread;

	// Register signals 
	signal(SIGQUIT, sig_handler);
	signal(SIGABRT, sig_handler);

	// Now that library is intialized the individual devices
	// can be intialized.
	sysInit();

	pthread_create(&generateAxleEventsThread,
		NULL,
		AxleEventThread,
		(void*)NULL);

	int printfDelay = 0;
	while(counting)
	{

		// Need to continuously update the dislpay
		// so it can be seen properly.
		int ledDisplayValue = trainBlockDetector[0].TrainAxleCount % 10000;
		display.SetDisplayValue(ledDisplayValue);

		for (int i = 0; i < 50; i++)
		{
			display.Display();
		}		
		printfDelay++;
		
		if (printfDelay % 1000)
		{
			printf("Speed: %f, Expected: %d, Actual: %d\n",
				speed,
				expectedAxleCount,
				trainBlockDetector[0].TrainAxleCount);
		}

		// For prototype display the state of the inputs
		for (int i = 0; i < numAxleSensors; i++)
		{
			axleSensors[i].RefreshOutputStatus();
		}		
	}
	Delay::Milliseconds(5000);

	pthread_join(generateAxleEventsThread, NULL);

	sysUninit();
	return 0;
}
