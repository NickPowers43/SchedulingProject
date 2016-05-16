#pragma once

#include "JobRun.h"
#include "JobData.h"
#include <vector>
#include <stack>

#define MAX_JOBS 16

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

	JobData jd;
	JobRun jr;
	JobRun* snapshot;

	int selectedServer;
	int selectedJob;

	int* selectedJobPtr;

	int selectedSyncPoint;
	int serverCount;
	int jobCount;
	int syncPointCount;

	float** colors;
	int colorCount;
};

