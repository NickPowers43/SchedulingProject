#include "stdafx.h"
#include "BruteIdleReducer.h"

#include <math.h>
#include <algorithm>

BruteIdleReducer::BruteIdleReducer()
{
}


BruteIdleReducer::~BruteIdleReducer()
{
}

static ReduceResults CalculateOptimal(vector<vector<ValType>> jobs, int remainingSyncPoints)
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

	size_t casesExplored = 0;
	vector<ValType> finiteCaseIdleTimes;

	bool ranOnce = false;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		if (jobs[i].size() > 1)
		{
			ranOnce = true;
			if (remainingSyncPoints == 0)
			{
				ReduceResults output;
				output.casesExplored = 1;
				output.idleTime = VAL_INF;
				return output;
			}

			ValType idleTime = VAL_ZERO;
			vector<vector<ValType>> jobs2;
			for (size_t j = 0; j < jobs.size(); j++)
			{
				if (jobs[j].size() > 1)
				{
					jobs2.push_back(vector<ValType>());
					if (j != i)
					{
						if (jobs[j].front() > jobs[i].front())
						{
							jobs2.back().push_back(jobs[j].front() - jobs[i].front());
						}
						else
						{
							idleTime += jobs[i].front() - jobs[j].front();
						}
					}
					for (size_t k = 1; k < jobs[j].size(); k++)
					{
						jobs2.back().push_back(jobs[j][k]);
					}
				}
			}

			//cut schedule at sync point at the end point of the first job of the ith server
			ReduceResults subResult = CalculateOptimal(jobs2, remainingSyncPoints - 1);

			for (size_t j = 0; j < subResult.finiteCaseTimes.size(); j++)
			{
				subResult.finiteCaseTimes[j] += idleTime;
			}

			casesExplored += subResult.casesExplored;
			finiteCaseIdleTimes.insert(finiteCaseIdleTimes.end(), subResult.finiteCaseTimes.begin(), subResult.finiteCaseTimes.end());

			if (subResult.idleTime < VAL_INF)
			{
				subResult.idleTime += idleTime;

				//sort
				if (subResult.idleTime < minResult.idletime)
				{
					minResult.idletime = subResult.idleTime;
					minResult.result = subResult;
					minResult.syncPoint = jobs[i].front();
				}
				//
			}

		}
	}

	minResult.result.finiteCaseTimes = finiteCaseIdleTimes;
	minResult.result.casesExplored = casesExplored;

	if (minResult.idletime != VAL_INF)
	{
		for (size_t i = 0; i < minResult.result.syncPoints.size(); i++)
		{
			minResult.result.syncPoints[i] += minResult.syncPoint;
		}
		minResult.result.syncPoints.push_back(minResult.syncPoint);
	}
	if (!ranOnce)
	{
		if (remainingSyncPoints == 0)
		{
			minResult.result.idleTime = VAL_ZERO;
			minResult.result.casesExplored = 1;
			minResult.result.finiteCaseTimes.push_back(VAL_ZERO);
		}
		else
		{
			minResult.result.idleTime = VAL_INF;
		}
	}

	return minResult.result;
}

void BruteIdleReducer::Reduce(vector<vector<ValType>> jobs, size_t syncPointCount)
{
	running = true;
	cancelled = false;

	syncPointCount = (syncPointCount < (jobs[0].size() - 1)) ? jobs[0].size() - 1 : syncPointCount;

	result = CalculateOptimal(jobs, syncPointCount);

	//sort(result.finiteCaseTimes.begin(), result.finiteCaseTimes.end());

	running = false;
}
