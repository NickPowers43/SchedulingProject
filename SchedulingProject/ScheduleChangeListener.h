#pragma once

#include "JobData.h"

class MiscScheduleWindow;

class ScheduleChangeListener
{
public:
	ScheduleChangeListener(MiscScheduleWindow* window);
	~ScheduleChangeListener();

	void Push(JobData &jd);

	MiscScheduleWindow* window;
};

