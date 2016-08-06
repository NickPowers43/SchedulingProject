#pragma once

#include "Scenario.h"
#include "ScheduleChangeListener.h"

class ScheduleFileWindow
{
public:
	ScheduleFileWindow(ScheduleChangeListener* changeListener);
	~ScheduleFileWindow();

	void OnGUI(Scenario & jd);

	ScheduleChangeListener* changeListener;

	char* filePath;
	bool canLoadFile;
};

