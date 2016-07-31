#pragma once

#include "JobData.h"
#include "ScheduleChangeListener.h"

class ScheduleFileWindow
{
public:
	ScheduleFileWindow(ScheduleChangeListener* changeListener);
	~ScheduleFileWindow();

	void OnGUI(JobData & jd);

	ScheduleChangeListener* changeListener;

	char* filePath;
	bool canLoadFile;
};

