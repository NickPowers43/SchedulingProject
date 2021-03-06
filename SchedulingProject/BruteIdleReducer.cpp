#include "stdafx.h"
#include "BruteIdleReducer.h"

#include <math.h>
#include <algorithm>
#include "imgui.h"

BruteIdleReducer::BruteIdleReducer()
{
	useT = false;
}


BruteIdleReducer::~BruteIdleReducer()
{
}

ReduceResults BruteIdleReducer::CalculateOptimal(Jobs jobs, int remainingSyncPoints, ValType t, double progressInc)
{
	//sorting variables
	struct SortingRecord
	{
		ValType idletime;//key
		ReduceResults result;
		ValType syncPoint;
	} min;
	//

	min.idletime = VAL_INF;
	min.result.idleTime = VAL_INF;
	min.result.casesExplored = 0;

	if (remainingSyncPoints == 0)
	{
		//we now have a complete configuration

		if (!useT)
		{
			if (jobs.MaxJobCount() > 1)
			{
				min.result.casesExplored = 1;
			}
			else
			{
				min.result.idleTime = VAL_ZERO;
				min.result.casesExplored = 1;
				min.result.finiteCaseTimes.push_back(VAL_ZERO);
			}
		}
		else
		{
			ValType idleTime = VAL_ZERO;
			for (size_t i = 0; i < jobs.serverCount(); i++)
			{
				if (jobs.jobCount(i) > 0)
				{
					ValType job = jobs.getJob(i, 0);
					if (job < t)
					{
						idleTime += t - job;
					}
				}
				else
				{
					idleTime += t;
				}
			}

			min.result.idleTime = idleTime;
			min.result.casesExplored = 1;
			min.result.finiteCaseTimes.push_back(idleTime);
		}

		IncProgress(progressInc);
	}
	else
	{
		size_t casesExplored = 0;
		vector<ValType> finiteCaseIdleTimes;

		float subProgressInc = progressInc / jobs.serverCount();

		int count = 0;
		for (size_t i = 0; i < jobs.serverCount(); i++)
		{
			if (jobs.jobCount(i) > 0)
			{
				ValType syncPoint = jobs.getJob(i, 0);

				if (useT)
				{
					if (syncPoint >= t)
					{
						continue;
					}
				}

				count++;

				Jobs jobs2;
				ValType idleTime = VAL_ZERO;
				jobs.jobsAfter(syncPoint, jobs2, idleTime, useT);

				//cut schedule at sync point at the end point of the first job of the ith server
				ReduceResults subResult = CalculateOptimal(jobs2, remainingSyncPoints - 1, t - syncPoint, subProgressInc);

				casesExplored += subResult.casesExplored;

				if (subResult.idleTime < VAL_INF)
				{
					subResult.idleTime += idleTime;

					for (size_t j = 0; j < subResult.finiteCaseTimes.size(); j++)
					{
						subResult.finiteCaseTimes[j] += idleTime;
					}
					finiteCaseIdleTimes.insert(finiteCaseIdleTimes.end(), subResult.finiteCaseTimes.begin(), subResult.finiteCaseTimes.end());

					//sort
					if (subResult.idleTime < min.idletime)
					{
						min.idletime = subResult.idleTime;
						min.result = subResult;
						min.syncPoint = syncPoint;
					}
					//
				}
			}
		}

		IncProgress(subProgressInc * (jobs.serverCount() - count));

		if (min.idletime < VAL_INF)
		{
			//finite configurations were found

			min.result.finiteCaseTimes = finiteCaseIdleTimes;

			for (size_t i = 0; i < min.result.syncPoints.size(); i++)
			{
				min.result.syncPoints[i] += min.syncPoint;
			}
			min.result.syncPoints.insert(min.result.syncPoints.begin(), min.syncPoint);
		}

		min.result.casesExplored = casesExplored;
	}

	return min.result;
}

void BruteIdleReducer::Reduce(Scenario scenario)
{
	running = true;
	cancelled = false;
	finished = false;
	SetProgress(0.0);

	//syncPointCount = (syncPointCount < (jobs[0].size() - 1)) ? jobs[0].size() - 1 : syncPointCount;

	useT = scenario.useT;

	result = CalculateOptimal(scenario.jobs, scenario.syncPoints.size(), scenario.t, 1.0);

	//sort(result.finiteCaseTimes.begin(), result.finiteCaseTimes.end());

	running = false;
	finished = true;
}
void BruteIdleReducer::OnGUI()
{
	//ImGui::Checkbox("Use T", &useT);
}
