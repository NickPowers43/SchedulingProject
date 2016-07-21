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

	vector<ValType> offsets;
	for (size_t i = 0; i < data.jobs.size(); i++)
	{
		offsets.push_back(0);
	}

	idleTime = 0.0f;
	for (size_t i = 0; i < data.jobs.size(); i++)
	{
		size_t syncI = 0;
		ValType idleStart = data.jobs[i].front();

		jobStarts[i][0] = 0;

		for (size_t jobI = 1; jobI < data.jobs[i].size(); jobI++)
		{
			while (syncI < data.syncPoints.size() && data.syncPoints[syncI] < idleStart)
			{
				syncI++;
			}

			if (syncI < data.syncPoints.size())
			{
				idleTime += data.syncPoints[syncI] - idleStart;//calculate idle time

				lastJob[i] = jobI + 1;
				jobStarts[i][jobI] = data.syncPoints[syncI];//set its start position to sync point
				idleStart = data.syncPoints[syncI] + data.jobs[i][jobI];//set next start position to end of this job
			}
			else
			{
				//no more sync points for this job
				idleTime = INFINITY;

				jobStarts[i][jobI] = idleStart;//set its start position to end of prev job
				idleStart += data.jobs[i][jobI];//set next start position to end of this job
			}
		}
	}
}
