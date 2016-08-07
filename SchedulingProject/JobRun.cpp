#include "stdafx.h"
#include "JobRun.h"
#include <algorithm>

using namespace std;

JobRun::JobRun()
{
}
JobRun::JobRun(Scenario & data) : data(data)
{
	for (size_t i = 0; i < data.jobs.serverCount(); i++)
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

	jobStarts = vector<vector<ValType>>();
	lastJob = vector<int>();
	
	//initialize job starts
	for (size_t i = 0; i < data.jobs.serverCount(); i++)
	{
		jobStarts.push_back(vector<ValType>());
		lastJob.push_back(0);

		for (size_t j = 0; j < data.jobs.jobCount(i); j++)
		{
			jobStarts.back().push_back(VAL_ZERO);
		}
	}

	idleTime = 0.0f;
	for (size_t i = 0; i < data.jobs.serverCount(); i++)
	{
		if (data.jobs.jobCount(i) > 0)
		{
			size_t syncI = 0;
			ValType idleStart = data.jobs.getJob(i, 0);

			jobStarts[i][0] = VAL_ZERO;

			for (size_t jobI = 1; jobI < data.jobs.jobCount(i); jobI++)
			{
				while (syncI < data.syncPoints.size() && data.syncPoints[syncI] < idleStart)
				{
					syncI++;
				}

				if (syncI < data.syncPoints.size())
				{
					ValType difference = data.syncPoints[syncI] - idleStart;
					idleTime += difference;//calculate idle time
					idleStart += difference;

					lastJob[i] = jobI + 1;
				}
				else
				{
					if (syncI == data.syncPoints.size())
					{
						//this will only happen once
						if (data.useT)
						{
							if (idleStart < data.t)
							{
								idleTime += data.t - idleStart;
								idleStart = data.t;
								lastJob[i] = jobI - 1;
							}
							else
							{
								lastJob[i] = jobI - 1;
							}
						}
						else
						{
							//no more sync points for this job
							idleTime = VAL_INF;
							lastJob[i] = jobI - 1;
						}
						syncI++;
					}
				}

				jobStarts[i][jobI] = idleStart;//set its start position
				idleStart += data.jobs.getJob(i, jobI);//set next start position to end of this job
			}

			if (data.useT && idleStart < data.t)
			{
				idleTime += data.t - idleStart;
			}
		}
		else
		{
			idleTime += data.t;
		}
	}
}
