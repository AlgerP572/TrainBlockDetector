#pragma once

class TrainBlockDetector
{
private:
	AxleSensor* _enterAxleCounter;

	float _trainScaleSpeedKmH;

	void AxleDetected(AxleType axleCount, float axleScaleSpeedKmH);

public:
	TrainBlockDetector(AxleSensor* enterAxleCounter);
	
	int TrainAxleCount;
	bool TrainPresent;
};