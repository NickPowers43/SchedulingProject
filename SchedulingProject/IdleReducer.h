#pragma once

#include <vector>

using namespace std;

struct ReduceResults
{
	ValType idleTime;
	vector<ValType> syncPoints;
	size_t casesExplored;
};

class IdleReducer
{
public:
	IdleReducer();
	virtual ~IdleReducer();

	virtual ReduceResults Reduce(vector<vector<ValType>> jobs, size_t syncPointCount);
};

