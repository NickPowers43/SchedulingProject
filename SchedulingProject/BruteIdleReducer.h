#pragma once

#include "IdleReducer.h"

class BruteIdleReducer :
	public IdleReducer
{
public:
	BruteIdleReducer();
	~BruteIdleReducer();

	ReduceResults Reduce(vector<vector<ValType>> jobs, size_t syncPointCount);
};

