#include "stdafx.h"
#include "OneExtraIdleReducer.h"
#include <math.h>
#include <algorithm>
#include "Min.h"

struct ServerRecord
{
	vector<ValType> jobs;
	bool crossed;
	bool firstJobStarted;

	void operator=(const ServerRecord &rhs)
	{
		jobs = rhs.jobs;
		crossed = rhs.crossed;
		firstJobStarted = rhs.firstJobStarted;
	}
};

bool SortDescendingByFirstEndPoint(const ServerRecord & a, const ServerRecord & b)
{
	if (a.firstJobStarted)
	{
		if (b.firstJobStarted)
		{
			return a.jobs.front() > b.jobs.front();
		}
		else
		{
			return true;
		}
	}
	else
	{
		if (b.firstJobStarted)
		{
			return false;
		}
		else
		{
			return false;
		}
	}
}

bool SortServersByFirstJobLength(const ServerRecord & a, const ServerRecord & b)
{
	return a.jobs.front() > b.jobs.front();
}

OneExtraIdleReducer::OneExtraIdleReducer()
{
}


OneExtraIdleReducer::~OneExtraIdleReducer()
{
}

vector<vector<ValType>> cutFirstAt(ValType at, vector<vector<ValType>> & src)
{
	vector<vector<ValType>> output;

	for (size_t i = 0; i < src.size(); i++)
	{
		output.push_back(vector<ValType>());

		if (src[i].front() > at)
		{
			output[i].push_back(src[i].front() - at);
		}

		for (size_t j = 1; j < src[i].size(); j++)
		{
			output[i].push_back(src[i][j]);
		}
	}

	return output;
}

template<typename T>
vector<vector<T>> removeFirst(vector<vector<T>> & src)
{
	vector<vector<T>> output;

	for (size_t i = 0; i < src.size(); i++)
	{
		output.push_back(vector<T>());
		for (size_t j = 1; j < src[i].size(); j++)
		{
			output[i].push_back(src[i][j]);
		}
	}

	return output;
}

template<typename T>
T maxFirst(vector<vector<T>> & src)
{
	vector<vector<T>> output;

	T max = VAL_ZERO;
	for (size_t i = 0; i < src.size(); i++)
	{
		if (src[i].size())
		{
			if (src[i][0] > max)
			{
				max = src[i][0];
			}
		}
	}

	return max;
}

vector<ServerRecord> ClipSchedule(vector<ServerRecord> & servers, ValType cutTime)
{
	vector<ServerRecord> output;

	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i].firstJobStarted)
		{
			if (servers[i].jobs.front() > cutTime)
			{
				output.push_back(ServerRecord());
				output.back().jobs = servers[i].jobs;
				output.back().jobs.front() -= cutTime;
				output.back().firstJobStarted = true;
				output.back().crossed = true;
			}
			else
			{
				if (servers[i].jobs.size() > 1)
				{
					output.push_back(ServerRecord());
					for (size_t j = 1; j < servers[i].jobs.size(); j++)
					{
						output.back().jobs.push_back(servers[i].jobs[j]);
					}
					output.back().firstJobStarted = false;
					output.back().crossed = servers[i].crossed;
				}
			}
		}
		else
		{
			output.push_back(ServerRecord());
			output.back().jobs = servers[i].jobs;
			output.back().firstJobStarted = true;
			output.back().crossed = servers[i].crossed;
		}
	}

	return output;
}

ReduceResults ReduceAdjust(vector<ServerRecord> servers)
{
	std::sort(servers.begin(), servers.end(), SortDescendingByFirstEndPoint);

	//sorting variables
	struct SortingRecord
	{
		ValType idletime = VAL_INF;//key
		ReduceResults result;
		ValType syncPoint;
	} minResult;
	//

	for (size_t i = 0; i < servers.size(); i++)
	{
		if (!servers[i].firstJobStarted)
		{
			break;
		}

		//cut schedule at sync point at the end point of the first job of the ith server
		ValType syncPoint = servers[i].jobs[0];
		vector<ServerRecord> subServers = ClipSchedule(servers, syncPoint);
		ReduceResults subResult = ReduceAdjust(subServers);

		//sort
		if (subResult.idleTime < minResult.idletime)
		{
			minResult.result = subResult;
			minResult.syncPoint = syncPoint;
		}
		//

		if (servers[i].crossed)
		{
			break;
		}

	}

	//shift sync points and insert best syncPoint
	for (size_t i = 0; i < minResult.result.syncPoints.size(); i++)
	{
		minResult.result.syncPoints[i] += minResult.syncPoint;
	}
	minResult.result.syncPoints.insert(minResult.result.syncPoints.begin(), minResult.syncPoint);
	return minResult.result;
}

ReduceResults OneExtraIdleReducer::Reduce(vector<vector<ValType>> jobs, size_t syncPointCount)
{
	vector<ServerRecord> servers;
	for (size_t i = 0; i < jobs.size(); i++)
	{
		servers.push_back(ServerRecord());
		servers[i].jobs = jobs[i];
		servers[i].crossed = false;
		servers[i].firstJobStarted = true;
	}

	//global sorting variables
	ValType minTotalIdletime = VAL_INF;		//key
	ReduceResults minTotalResult;			//value
	vector<ValType> minPreSyncPoints;		//value
	//

	//information about the past schedule
	ValType preIdleTime = VAL_ZERO;
	ValType sectionBeginTime = VAL_ZERO;
	vector<ValType> syncPoints;
	//

	for (size_t i = 0; i < (jobs[0].size() - 1); i++)
	{
		std::sort(servers.begin(), servers.end(), SortDescendingByFirstEndPoint);

		ValType tSectionIdletime = VAL_ZERO;
		for (size_t j = 1; j < servers.size(); j++)
		{
			tSectionIdletime += servers[0].jobs[0] - servers[j].jobs[0];
		}
		ValType originalTIdletime = tSectionIdletime;

		//sorting variables
		ValType minIdleTime = VAL_INF;			//key
		ReduceResults minResult;				//value
		ValType minSyncPoint;					//value
		//

		for (size_t j = 1; j < servers.size(); j++)
		{
			if (j > 0)
			{
				tSectionIdletime -= (servers[j-1].jobs[0] - servers[j].jobs[0]) * (servers.size() - j);
			}

			//cut schedule at sync point at the end point of the first job of the ith server
			ValType syncPoint = servers[j].jobs[0];
			vector<ServerRecord> subServers = ClipSchedule(servers, syncPoint);
			ReduceResults subResult = ReduceAdjust(subServers);

			//sort
			if (minIdleTime > subResult.idleTime)
			{
				minIdleTime = subResult.idleTime;
				minResult = subResult;
				minSyncPoint = syncPoint;
			}
			//
		}
		minResult.idleTime += preIdleTime;

		//global sort
		if (minTotalIdletime > minResult.idleTime)
		{
			minTotalResult = minResult;
			minPreSyncPoints = syncPoints;
		}
		//

		//advance the past variables
		preIdleTime += originalTIdletime;
		sectionBeginTime += servers[0].jobs[0];
		syncPoints.push_back(((syncPoints.size()) ? syncPoints.back() : VAL_ZERO) + servers[0].jobs[0]);
		//

		//remove the first jobs of each server so that we advance to the next section
		for (size_t j = 0; j < servers.size(); j++)
		{
			servers[j].jobs.erase(servers[j].jobs.begin());
		}
	}
	
	minTotalResult.syncPoints.insert(minTotalResult.syncPoints.begin(), minPreSyncPoints.begin(), minPreSyncPoints.end());
	return minTotalResult;
}
