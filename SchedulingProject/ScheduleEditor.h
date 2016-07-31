#pragma once

#include "JobData.h"
#include "JobRun.h"

#include "ScheduleChangeListener.h"

class ScheduleEditor
{
public:
	ScheduleEditor(ScheduleChangeListener* changeListener);
	~ScheduleEditor();

	void OnGUI(JobData & jd);
	void DrawJobRun(JobRun & jobRun);

	JobRun jr;

	float** colors;
	int colorCount;

	int selectedServer;
	int selectedJob;
	int* selectedJobPtr;
	int selectedSyncPoint;

	ScheduleChangeListener* changeListener;

	JobRun* snapshot;
};

