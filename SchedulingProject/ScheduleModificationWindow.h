#pragma once

#include "JobData.h"

class ScheduleModificationWindow
{
public:
	ScheduleModificationWindow();
	~ScheduleModificationWindow();

	void OnGUI(JobData & jd);
};

