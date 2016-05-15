#include "stdafx.h"
#include "JobRun.h"
#include <algorithm>

using namespace std;

bool FCompare(float i, float j) { return (i<j); }

JobRun::JobRun()
{
}
JobRun::JobRun(JobData & data, vector<float> & syncPoints) : data(data), jobStarts(data.jobs), syncPoints(syncPoints)
{
	for (size_t i = 0; i < data.jobs.size(); i++)
	{
		lastJob.push_back(0);
	}
	Simulate();
}

JobRun::~JobRun()
{
}

void JobRun::Simulate()
{
	sort(syncPoints.begin(), syncPoints.end());

	idleTime = 0.0f;
	for (size_t i = 0; i < data.jobs.size(); i++)
	{
		int k = 0;
		jobStarts[i][0] = 0.0f;
		float start = 0.0f;
		lastJob[i] = data.jobs[i].size();
		for (size_t j = 1; j < data.jobs[i].size(); j++)
		{
			start = start + data.jobs[i][j - 1];

			while (k < syncPoints.size() && syncPoints[k] < start)
			{
				k++;
			}
			if (k >= syncPoints.size())
			{
				if (j < lastJob[i])
				{
					lastJob[i] = j;
				}

				idleTime = INFINITY;
			}
			else
			{
				float temp = syncPoints[k] - start;
				idleTime += temp;
				start += temp;
			}

			jobStarts[i][j] = start;
		}

	}
}
