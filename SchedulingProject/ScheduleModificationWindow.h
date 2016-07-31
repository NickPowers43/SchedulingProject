#pragma once

#include "JobData.h"

class ScheduleModificationWindow
{
public:
	ScheduleModificationWindow();
	~ScheduleModificationWindow();

	void (*modifyCallback)(JobData & jd);

	void RandomizePoisson(JobData & jd);
	void RandomizeUniform(JobData & jd);
	void OnGUI(JobData & jd);

	float uniformMin;
	float uniformMax;
};

