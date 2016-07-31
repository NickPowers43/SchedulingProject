#pragma once

#include "JobData.h"

class ScheduleFileWindow
{
public:
	ScheduleFileWindow();
	~ScheduleFileWindow();

	void OnGUI(JobData & jd);

	char* filePath;
	bool canLoadFile;
};

