#pragma once

#include "JobRun.h"
#include "JobData.h"
#include <vector>
#include <stack>

using namespace std;

class ScheduleViewer
{
public:
	ScheduleViewer();
	~ScheduleViewer();

	void OnGUI();

private:

	vector<float> syncPoints;
	stack<JobData> saves;

	JobData jd;
	JobRun jr;

	int selectedServer;
	int selectedJob;

	float* selectedJobPtr;

	int selectedSyncPoint;
	int serverCount;
	int jobCount;
	int syncPointCount;

	unsigned int* colors;
	int colorCount;
};

