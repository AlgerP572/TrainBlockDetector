#pragma once

class TrainBlockDetector
{
private:
	AxleSensor* _enterAxleCounter;

	float _trainScaleSpeedKmH;

	void CalculateSpeed(int axleCount, float axleScaleSpeedKmH);

public:
	TrainBlockDetector(AxleSensor* enterAxleCounter);
	
	int TrainAxleCount;
	bool TrainPresent;
};