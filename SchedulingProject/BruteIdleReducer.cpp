#include "stdafx.h"
#include "BruteIdleReducer.h"
#include <math.h>

BruteIdleReducer::BruteIdleReducer()
{
}


BruteIdleReducer::~BruteIdleReducer()
{
}

static ValType CalculateOptimal(vector<vector<ValType>> jobs, vector<ValType>& syncPoints, int remainingSyncPoints)
{
	bool ranOnce = false;
	ValType bestSyncPoint = VAL_INF;
	ValType bestIdleTime = VAL_INF;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		if (jobs[i].size() > 1)
		{
			ranOnce = true;
			if (remainingSyncPoints == 0)
			{
				return VAL_INF;
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

			vector<ValType> temp;
			ValType subIdleTime = CalculateOptimal(jobs2, temp, remainingSyncPoints - 1);
			if (subIdleTime != VAL_INF)
			{
				idleTime += subIdleTime;

				if (idleTime < bestIdleTime)
				{
					bestSyncPoint = jobs[i].front();
					bestIdleTime = idleTime;
					syncPoints = temp;
				}
			}

		}
	}

	if (bestSyncPoint != VAL_INF)
	{
		for (size_t i = 0; i < syncPoints.size(); i++)
		{
			syncPoints[i] += bestSyncPoint;
		}
		syncPoints.push_back(bestSyncPoint);
		return bestIdleTime;
	}
	if (ranOnce)
	{
		return VAL_INF;
	}
	else
	{
		return VAL_ZERO;
	}

}

ReduceResults BruteIdleReducer::Reduce(vector<vector<ValType>> jobs, size_t syncPointCount)
{
	syncPointCount = (syncPointCount < (jobs[0].size() - 1)) ? jobs[0].size() - 1 : syncPointCount;

	ReduceResults output;
	output.casesExplored = 0;
	output.idleTime = CalculateOptimal(jobs, output.syncPoints, syncPointCount);
	return output;
}
