#pragma once

#include "JobData.h"
#include "ScheduleChangeListener.h"

class ScheduleModificationWindow
{
public:
	ScheduleModificationWindow(ScheduleChangeListener* changeListener);
	~ScheduleModificationWindow();

	void (*modifyCallback)(JobData & jd);

	void RandomizeNormal(JobData & jd);
	void RandomizeUniform(JobData & jd);
	void Constant(JobData & jd);
	void OnGUI(JobData & jd);

	float uniformMin;
	float uniformMax;

	float normalMean;
	float normalSigma;

	ScheduleChangeListener* changeListener;

	float constant;
};

