#pragma once

#include "JobRun.h"
#include "JobData.h"
#include <vector>

using namespace std;

class ScheduleViewer
{
public:
	ScheduleViewer();
	~ScheduleViewer();

	void OnGUI();

private:

	vector<float> syncPoints;

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

