#pragma once

#include "JobRun.h"
#include "JobData.h"
#include <vector>
#include <stack>
#include "ReduceWindow.h"
#include "ScheduleEditor.h"
#include "ScheduleFileWindow.h"
#include "ScheduleModificationWindow.h"

using namespace std;

class ScheduleViewer
{
public:
	ScheduleViewer();
	~ScheduleViewer();

	void OnGUI();

	void DrawJobRun(JobRun & jobRun);

private:

	stack<JobData> saves;
	ReduceWindow* reduceWindow;
	ScheduleEditor* editorWindow;
	ScheduleFileWindow* fileWindow;
	ScheduleModificationWindow* modificationWindow;

	JobData jd;

	UserSettings settings;

};

