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
	} minResult;
	//

	minResult.idletime = VAL_INF;
	minResult.result.idleTime = VAL_INF;
	minResult.result.casesExplored = 0;

	if (remainingSyncPoints == 0)
	{
		//we now have a complete configuration

		if (!useT)
		{
			if (jobs.MaxJobCount() > 1)
			{
				minResult.result.casesExplored = 1;
			}
			else
			{
				minResult.result.idleTime = VAL_ZERO;
				minResult.result.casesExplored = 1;
				minResult.result.finiteCaseTimes.push_back(VAL_ZERO);
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

			minResult.result.idleTime = idleTime;
			minResult.result.casesExplored = 1;
			minResult.result.finiteCaseTimes.push_back(idleTime);
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
				jobs.jobsAfter(syncPoint, jobs2, idleTime);

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
					if (subResult.idleTime < minResult.idletime)
					{
						minResult.idletime = subResult.idleTime;
						minResult.result = subResult;
						minResult.syncPoint = syncPoint;
					}
					//
				}
			}
		}

		IncProgress(subProgressInc * (jobs.serverCount() - count));

		if (minResult.idletime < VAL_INF)
		{
			//finite configurations were found

			minResult.result.finiteCaseTimes = finiteCaseIdleTimes;

			for (size_t i = 0; i < minResult.result.syncPoints.size(); i++)
			{
				minResult.result.syncPoints[i] += minResult.syncPoint;
			}
			minResult.result.syncPoints.insert(minResult.result.syncPoints.begin(), minResult.syncPoint);
		}

		minResult.result.casesExplored = casesExplored;
	}

	return minResult.result;
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
