#pragma once

class TrainBlockDetector
{
private:
	AxleCounter* _enterAxleCounter;

	float _trainScaleSpeedKmH;

	void CalculateSpeed(int axleCount, float axleScaleSpeedKmH);

public:
	TrainBlockDetector(AxleCounter* enterAxleCounter);
	
	int TrainAxleCount;
	bool TrainPresent;
};