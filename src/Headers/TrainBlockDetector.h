#pragma once

class TrainBlockDetector
{
private:
	AxleSensor* _enterAxleCounter;

	void AxleDetected(AxleType axleCount, float axleScaleSpeedKmH);

public:
	TrainBlockDetector(AxleSensor* enterAxleCounter);
	
	int TrainAxleCount;
	int BlockAxleCount;
	float TrainScaleSpeedKmH;
	bool TrainPresent;
	void Reset();
};