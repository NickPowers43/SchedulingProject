#pragma once

#include "JobData.h"

class ReduceWindow
{
public:
	ReduceWindow();
	~ReduceWindow();

	void OnGUI(JobData& jd);

	int reducerPreference;
	ValType idleTime;
	int finiteCases;

	vector<ValType> finiteCaseTimes;

	char* filePath;
};

