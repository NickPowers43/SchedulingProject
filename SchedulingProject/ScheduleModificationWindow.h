#pragma once

#include "Scenario.h"
#include "ScheduleChangeListener.h"

class ScheduleModificationWindow
{
public:
	ScheduleModificationWindow(ScheduleChangeListener* changeListener);
	~ScheduleModificationWindow();

	void (*modifyCallback)(Scenario & jd);

	void RandomizeNormal(Scenario & jd);
	void RandomizeUniform(Scenario & jd);
	void Constant(Scenario & jd);
	void OnGUI(Scenario & jd);

	float uniformMin;
	float uniformMax;

	float normalMean;
	float normalSigma;

	ScheduleChangeListener* changeListener;

	float constant;
};

