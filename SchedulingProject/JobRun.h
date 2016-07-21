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
	vector<vector<ValType>> jobStarts;
	vector<ValType> lastJob;

	ValType idleTime;
};

