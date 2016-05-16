#include "stdafx.h"
#include "JobRun.h"
#include <algorithm>

using namespace std;

JobRun::JobRun()
{
}
JobRun::JobRun(JobData & data) : data(data), jobStarts(data.jobs)
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
	sort(data.syncPoints.begin(), data.syncPoints.end());

	idleTime = 0.0f;
	for (size_t i = 0; i < data.jobs.size(); i++)
	{
		int k = 0;
		jobStarts[i][0] = 0.0f;
		int start = 0.0f;
		lastJob[i] = data.jobs[i].size();
		for (size_t j = 1; j < data.jobs[i].size(); j++)
		{
			start = start + data.jobs[i][j - 1];

			while (k < data.syncPoints.size() && data.syncPoints[k] < start)
			{
				k++;
			}
			if (k >= data.syncPoints.size())
			{
				if (j < lastJob[i])
				{
					lastJob[i] = j;
				}

				idleTime = INFINITY;
			}
			else
			{
				float temp = data.syncPoints[k] - start;
				idleTime += temp;
				start += temp;
			}

			jobStarts[i][j] = start;
		}

	}
}
