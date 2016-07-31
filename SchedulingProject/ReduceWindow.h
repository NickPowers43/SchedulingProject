#pragma once

#include "JobData.h"
#include "ScheduleChangeListener.h"

class ReduceWindow
{
public:
	ReduceWindow(ScheduleChangeListener* changeListener);
	~ReduceWindow();

	void OnGUI(JobData& jd);

	int reducerPreference;
	ValType idleTime;
	int finiteCases;
	int totalCases;

	vector<ValType> finiteCaseTimes;

	ScheduleChangeListener* changeListener;

	char* filePath;
};

