#pragma once

#include "JobData.h"
#include "JobRun.h"

class ScheduleEditor
{
public:
	ScheduleEditor();
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

	JobRun* snapshot;
};

