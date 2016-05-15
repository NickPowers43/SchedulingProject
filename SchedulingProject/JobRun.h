#pragma once

#include "JobData.h"

class JobRun
{
public:
	JobRun();
	JobRun(JobData & data);
	~JobRun();

	void Simulate();

	JobData data;
	vector<vector<float>> jobStarts;
	vector<int> lastJob;

	float idleTime;
};

