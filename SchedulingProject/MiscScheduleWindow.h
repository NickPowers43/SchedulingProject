#pragma once

#include <stack>
#include "JobData.h"

#include "ScheduleChangeListener.h"

class MiscScheduleWindow
{
public:
	MiscScheduleWindow();
	~MiscScheduleWindow();

	void Push(JobData & jd);
	void OnGUI(JobData & jd);
	ScheduleChangeListener* GetListener();

	ScheduleChangeListener listener;
	stack<JobData> undoes;
	stack<JobData> redoes;
};

