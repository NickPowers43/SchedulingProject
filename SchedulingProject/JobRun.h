#pragma once

#include "JobData.h"

class JobRun
{
public:
	JobRun();
	JobRun(JobData & data, vector<float> & syncPoints);
	~JobRun();

	void Simulate();

	JobData data;
	vector<float> syncPoints;
	vector<vector<float>> jobStarts;
	vector<int> lastJob;

	float idleTime;
};

