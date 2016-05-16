#pragma once

#include "JobData.h"

class JobRun
{
public:
	JobRun();
	JobRun(JobData & data);
	~JobRun();

	void Simulate();
	float IdleTime();


	JobData data;
	vector<vector<int>> jobStarts;
	vector<int> lastJob;

	int idleTime;
};

