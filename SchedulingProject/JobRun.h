#pragma once

#include "Scenario.h"

class JobRun
{
public:
	JobRun();
	JobRun(Scenario & data);
	~JobRun();

	void Simulate();

	Scenario data;
	vector<vector<ValType>> jobStarts;
	vector<ValType> lastJob;

	ValType idleTime;
};

